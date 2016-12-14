from run_lib import *
from memory_lib import *
import threading
from time import sleep


################################################################################



def worker():

    name = threading.current_thread().getName()
    while(1):
        sleep(10)
        for program in program_to_run[name]:
            program_counter= memory[program]['pc']
            current_command = command[program][program_counter]
            state[program]='RUNNING'
            run_command(program,current_command,program_counter)
            state[program]='BLOCKED'
            if (current_command[0]=='RETURN'):
                program_to_run[name].remove(program)
                state[program]='DONE'
                print(program_to_run)

            




def interpreter():
    interpreter_mode = ['EXIT','STATE']
    i=0
    p_id=-1

    while(1):
        exec_input = input('exec> ').split()
    
        if (exec_input[0] not in interpreter_mode):

            file_code = exec_input[0]+'.txt'

            p_id = p_id+1
            parser(file_code,p_id)

            state[p_id]='Blocked'
            pid_to_name[p_id]=exec_input[0]

            print(state)
            print(pid_to_name)

            arguments(exec_input,p_id)
            find_labels(p_id)
            insert_to_mem(p_id,'pc',0)

            thread_name = 'Thread-'+str(i)
       
            if (thread_name in program_to_run.keys()):
                program_to_run[thread_name].append(p_id)
            else:
                program_to_run[thread_name]=[p_id]

            t = threading.Thread(name=thread_name,target=worker)
            t.start()

        elif (exec_input[0]=='STATE'):
            print("{:10} {:10} {:10}".format('Pid','Name','STATE'))
            for key in pid_to_name.keys():
                print("{:10} {:10} {:10}".format(str(key),pid_to_name[key],state[key]))


state={}
pid_to_name={}
program_to_run = {}
interpreter()
