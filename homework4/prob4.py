import threading
from time import * 
from tkinter import *
from run_lib import *
from memory_lib import *
import sys
from random import randint
################################################################################

def next_program(name):
    run_lock.acquire()

    length = len(program_to_run[name])

    if (length>=1):
        
        #list starts from 0-position!
        index = randint(0,length-1)

        run_lock.release()
        return program_to_run[name][index]

    run_lock.release()
    return None

def remove_program(name,program):

    run_lock.acquire()
    program_to_run[name].remove(program)
    run_lock.release() 

def change_state(program,new_state):
    #debug = open('states.txt','a')

    state_lock.acquire()
    if (state[program] == 'KILLED'):
        print('State of program is KILLED')
    elif (state[program]!='DONE'):
        state[program]=new_state
        #debug.write(str(state))
        #debug.write('\n')
    if (state[program] == 'KILLED'):
        print('State of program is KILLED')

    #debug.close()
    state_lock.release()



#check for a program if the sleeping time is passed
def wake_up(name):

    end_time = time.time()
        
    for program in program_to_run[name]:
        if (state[program]=='SLEEP'):
            if (end_time-sleeping_program[program][1]>=sleeping_program[program][0]):
                change_state(program,'BLOCKED')



def terminate():
    global terminate_threads

    if (terminate_threads==True):
        quit()

#threads runnings programs
def worker():
    global window_output
    name = threading.current_thread().getName()
    while(1):
        terminate()

        program=next_program(name)
        
        #there is no program to run 
        if (program==None):
            continue
            
        wake_up(name)
        if (state[program]=='SLEEP'):
            continue

        program_counter= memory[program]['pc']
        current_command = command[program][program_counter]
        change_state(program,'RUNNING')

        run_command(program,current_command,program_counter,window_output)

        
        change_state(program,'BLOCKED')

        if (current_command[0]=='RETURN'):
            remove_program(name,program)
            change_state(program,'DONE')
        elif (current_command[0]=='SLEEP'):
            sleeping_program[program]= [var_or_value(program,current_command[1]),time.time()] 
            change_state(program,'SLEEP')
    
#wait for user to give a program,assign it to workers!
def interpreter():
    
    interpreter_mode = ['EXIT','STATE','KILL']
    total_threads=0
    max_threads = int(sys.argv[1])
    p_id=-1
    reach_max_threads=False

    while(1):
        exec_input = input('exec> ').split()
        if (not exec_input):
            continue

        if (exec_input[0] not in interpreter_mode):

            file_code = exec_input[0]+'.txt'

            p_id = p_id+1
            parser(file_code,p_id)

            state[p_id]='BLOCKED'

            insert_arguments(exec_input,p_id)
            find_labels(p_id)
            insert_to_mem(p_id,'pc',0)
            
            #reach maximum threads .Restart total threads!
            if (total_threads==max_threads):
                total_threads=0
                reach_max_threads = True

            thread_name = 'Thread-'+str(total_threads)
            total_threads +=1
       
            run_lock.acquire()
            if (thread_name in program_to_run.keys()):
                program_to_run[thread_name].append(p_id)
            else:
                program_to_run[thread_name]=[p_id]
            run_lock.release()
    
            pid_to_name[p_id]=[exec_input[0],thread_name]
            #create new thread!
            if (reach_max_threads==False):
                t = threading.Thread(name=thread_name,target=worker)
                thread_list.append(t)
                t.start()

        elif (exec_input[0]=='STATE'):
            state_lock.acquire()
            print("{:10} {:10} {:10}\n".format('Pid','Name','Thread','STATE'))
            for key in pid_to_name.keys():
                print("{:10} {:10} {:10} {:10}\n".format(str(key),pid_to_name[key][0],pid_to_name[key][1],state[key]))
            state_lock.release()

        elif (exec_input[0]=='KILL'):
            if (len(exec_input) == 2):
                exec_input[1] = int(exec_input[1])
                name_of_thread = 'Thread-'+ str(exec_input[1]%max_threads)
                if (exec_input[1] in program_to_run[name_of_thread]):
                    remove_program(name_of_thread,exec_input[1])
                    change_state(exec_input[1],'KILLED')
            else:
                print ('You forgot to pass as argument the pid of the program')

        else:
            global terminate_threads
            terminate_threads= True
            for thread in thread_list:
                thread.join()
            print('Bye ♪~ ᕕ(ᐛ)ᕗ')
            return None


#unblock from root.mainloop()
def mainloop_quit():
    global root
    root.quit()


#initialize structures !
terminate_threads = False
thread_list = []
run_lock = threading.Lock()
state_lock = threading.Lock()
sleeping_program = {}
state={}
pid_to_name={}
program_to_run = {}


#Create a window for programs output!
root = Tk()
window_output = Text(root)
root.title("When you are bored as hell!")
Label(text='Program Output').pack()
window_output.pack()
Button(root, text="Quit", command=mainloop_quit).pack()

interpreter_thread = threading.Thread(target=interpreter)
interpreter_thread.start()

root.mainloop()

interpreter_thread.join()
