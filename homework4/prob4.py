import threading
from time import * 
from tkinter import *
from run_lib import *
from memory_lib import *
import sys
################################################################################

#check for a program if the sleeping time is passed
def wake_up(name):

    end_time = time.time()

    for i in range (len(program_to_run[name])):
        run_lock.acquire()
        program = program_to_run[name][i]
        run_lock.release()
        if (state[program]=='SLEEP'):
            if (end_time-sleeping_program[program][1]>=sleeping_program[program][0]):
                mtx.acquire()
                state[program]='BLOCKED'
                mtx.release()

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
        for i in range (len(program_to_run[name])):

            run_lock.acquire()
            program = program_to_run[name][i]
            run_lock.release()

            wake_up(name)
            if (state[program]=='SLEEP'):
                continue

            program_counter= memory[program]['pc']
            current_command = command[program][program_counter]
            mtx.acquire()
            state[program]='RUNNING'
            mtx.release()
            run_command(program,current_command,program_counter,window_output)
            mtx.acquire()
            state[program]='BLOCKED'
            mtx.release()
            if (current_command[0]=='RETURN'):
                run_lock.acquire()
                program_to_run[name].remove(program)
                run_lock.release()
                mtx.acquire()
                state[program]='DONE'
                mtx.release()
            elif (current_command[0]=='SLEEP'):
                sleeping_program[program]= [var_or_value(program,current_command[1]),time.time()] 
                mtx.acquire()
                state[program]='SLEEP'
                mtx.release()

#wait for user to give a program,assign it to workers!
def interpreter():
    
    interpreter_mode = ['EXIT','STATE','KILL']
    total_threads=0
    max_threads = int(sys.argv[1])
    p_id=-1
    reach_max_threads=False

    while(1):
        exec_input = input('exec> ').split()
    
        if (exec_input[0] not in interpreter_mode):

            file_code = exec_input[0]+'.txt'

            p_id = p_id+1
            parser(file_code,p_id)

            state[p_id]='Blocked'

            insert_arguments(exec_input,p_id)
            find_labels(p_id)
            insert_to_mem(p_id,'pc',0)
            
            #reach maximum threads .Restart total threads!
            if (total_threads==max_threads):
                total_threads=0
                reach_max_threads = True

            thread_name = 'Thread-'+str(total_threads)
            total_threads +=1
       
            if (thread_name in program_to_run.keys()):
                program_to_run[thread_name].append(p_id)
            else:
                program_to_run[thread_name]=[p_id]
    
            pid_to_name[p_id]=[exec_input[0],thread_name]
            #create new thread!
            if (reach_max_threads==False):
                t = threading.Thread(name=thread_name,target=worker)
                thread_list.append(t)
                t.start()

        elif (exec_input[0]=='STATE'):
            mtx.acquire()
            print("{:10} {:10} {:10}\n".format('Pid','Name','Thread','STATE'))
            for key in pid_to_name.keys():
                print("{:10} {:10} {:10} {:10}\n".format(str(key),pid_to_name[key][0],pid_to_name[key][1],state[key]))
            mtx.release()

        elif (exec_input[0]=='KILL'):
            exec_input[1] = int(exec_input[1])
            name_of_thread = 'Thread-'+ str(exec_input[1]%max_threads)
            if (exec_input[1] in program_to_run[name_of_thread]):
                run_lock.acquire()
                program_to_run[name_of_thread].remove(exec_input[1])
                run_lock.release()
                mtx.acquire()
                state[exec_input[1]]='KILLED'
                mtx.release()

        else:
            global terminate_threads
            terminate_threads= True
            for thread in thread_list:
                thread.join()
            print('Bye :)')
            return None


#unblock from root.mainloop()
def mainloop_quit():
    global root
    root.quit()


#initialize structures !
terminate_threads = False
thread_list = []
run_lock = threading.Lock()
mtx = threading.Lock()
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
