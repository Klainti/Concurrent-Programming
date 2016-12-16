from run_lib import *
from memory_lib import *
import threading
from time import sleep
import sys
################################################################################



#threads runnings programs
def worker():

    name = threading.current_thread().getName()
    while(1):
        for program in program_to_run[name]:
            program_counter= memory[program]['pc']
            current_command = command[program][program_counter]
            state[program]='RUNNING'
            run_command(program,current_command,program_counter)
            print(global_memory)
            state[program]='BLOCKED'
            if (current_command[0]=='RETURN'):
                program_to_run[name].remove(program)
                state[program]='DONE'
           
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

            arguments(exec_input,p_id)
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
                t.start()

        elif (exec_input[0]=='STATE'):
            print("{:10} {:10} {:10}".format('Pid','Name','Thread','STATE'))
            for key in pid_to_name.keys():
                print("{:10} {:10} {:10} {:10}".format(str(key),pid_to_name[key][0],pid_to_name[key][1],state[key]))

        elif (exec_input[0]=='KILL'):
            exec_input[1] = int(exec_input[1])
            name_of_thread = 'Thread-'+ str(exec_input[1]%max_threads)
            if (exec_input[1] in program_to_run[name_of_thread]):
                program_to_run[name_of_thread].remove(exec_input[1])
                print(program_to_run)
                state[exec_input[1]]='KILLED'

state={}
pid_to_name={}
program_to_run = {}
interpreter()
