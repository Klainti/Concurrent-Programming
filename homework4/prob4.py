from run_lib import *
from memory_lib import *


################################################################################



def main():

    while(1):
        if (name_of_programs==[]): 

            #read the code
            exec_input = input('exec> ').split()
            file_code = exec_input[0]+'.txt'


            parser(file_code)
    
            p_id = name_of_programs.index(file_code.split('.')[0])
            arguments(exec_input,p_id)
            find_labels(name_of_programs[p_id])
            insert_to_mem(name_of_programs[p_id],'pc',0)

        while(name_of_programs!=[]):
            program_counter = memory[name_of_programs[p_id]]['pc']
            run_command(name_of_programs[p_id],command[name_of_programs[p_id]][program_counter],program_counter)
main()
