from memory_lib import *
import time


def ADD(var_name,var1,var2,name_of_program):
    insert_to_mem(name_of_program,var_name,var1+var2)
    return None

def SUB(var_name,var1,var2,name_of_program):
    insert_to_mem(name_of_program,var_name,var1-var2)
    return None

def MUL(var_name,var1,var2,name_of_program):
    insert_to_mem(name_of_program,var_name,var1*var2)
    return None

def DIV(var_name,var1,var2,name_of_program):
    insert_to_mem(name_of_program,var_name,int(var1/var2))
    return None

def MOD(var_name,var1,var2,name_of_program):
    insert_to_mem(name_of_program,var_name,var1%var2)
    return None

def SET(var_name,var,name_of_program):
    insert_to_mem(name_of_program,var_name,var)
    return None

def BRGT(var1,var2,label,pc,name_of_program):

    if (var1>var2):
        return label
    else:
        return pc+1

def BRGE(var1,var2,label,pc,name_of_program):

    if (var1>=var2):
        return label
    else:
        return pc+1

def BRLT(var1,var2,label,pc,name_of_program):

    if (var1<var2):
        return label
    else:
        return pc+1

def BRLE(var1,var2,label,pc,name_of_program):

    if (var1<=var2):
        return label
    else:
        return pc+1

def BREQ(var1,var2,label,pc,name_of_program):

    if (var1==var2):
        return label
    else:
        return pc+1

def BRA(label,name_of_program):
    
    return label

def SLEEP(var):
    time.sleep(var)
    return None

def PRINT(name_of_program,command_list):
    string = command_list[1]

    length_of_variables = len(command_list)

    for i in range(2,length_of_variables):
        value = var_or_value(name_of_program,command_list[i])
        string = string +' '+ str(value)

    print(string)
    return None

def LOAD(var_name,global_var_name,name_of_program):
    global_value = read_from_global_mem(global_var_name)
    SET(var_name,global_value,name_of_program)
    return None

def STORE(global_var_name,var_val):
    insert_to_global_mem(global_var_name,var_val)
    return None

def RETURN(name_of_program):
    del memory[name_of_program]
    del command[name_of_program]
    return None

def DOWN(sem_name,sem_value,pc):
    
    if (sem_value!=0):
        sem_value = sem_value - 1
        insert_to_global_mem(sem_name,sem_value)
        return pc+1
    else:
        return pc

def UP(sem_name,sem_value):
    sem_value = sem_value + 1
    insert_to_global_mem(sem_name,sem_value)
