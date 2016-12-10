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

def RETURN(name_of_program):
    del memory[name_of_program]
    del command[name_of_program]
    name_of_programs.pop(name_of_programs.index(name_of_program))

#read all lines from .txt
def parser(program):
    fd = open(program,'r')
    name_of_programs.append(fd.readline().strip('\n'))

    p_id = name_of_programs.index(program)
 
    #split each line of the program 
    tmp_list = []
    for line in fd.readlines():
        tmp_list.append(line.strip('\n').split())
        command[name_of_programs[p_id]] = tmp_list

#check and saves to memory labels and their lines in .txt
def find_labels(name_of_program):

    #check if there is a label in commands!
    for i in range(0,len(command[name_of_program])):
        label = command[name_of_program][i][0]
        if label not in list_of_commands:
            insert_to_mem(name_of_program,label,i)

            #remove label from commands
            command[name_of_program][i].pop(0)

#insert variable and their values to memory
def insert_to_mem(name_of_program,key_var,value):
    if (name_of_program in memory.keys()):
        memory[name_of_program][key_var] = value
    else:
        memory[name_of_program] = {key_var: value}

#check if variable is in memory or raw value from .txt
def var_or_value(name_of_program,variable):

    if (variable[0]=='$'):
        return memory[name_of_program][variable]
    else:
        return int(variable)

def run_command(name_of_program,command_list,pc):
    
    if (command_list[0]=='ADD'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        ADD(var_name,var1,var2,name_of_program)
        insert_to_mem(name_of_program,'pc',pc+1)
    elif (command_list[0]=='SUB'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        SUB(var_name,var1,var2,name_of_program)
        insert_to_mem(name_of_program,'pc',pc+1)
    elif (command_list[0]=='MUL'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        MUL(var_name,var1,var2,name_of_program)
        insert_to_mem(name_of_program,'pc',pc+1)
    elif (command_list[0]=='DIV'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        DIV(var_name,var1,var2,name_of_program)
        insert_to_mem(name_of_program,'pc',pc+1)
    elif (command_list[0]=='MOD'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        MOD(var_name,var1,var2,name_of_program)
        insert_to_mem(name_of_program,'pc',pc+1)
    elif(command_list[0]=='SET'):
        var_name= command_list[1]
        var = var_or_value(name_of_program,command_list[2])
        SET(var_name,var,name_of_program)
        insert_to_mem(name_of_program,'pc',pc+1)
    elif(command_list[0]=='BRGT'):
        var1 = var_or_value(name_of_program,command_list[1])
        var2 = var_or_value(name_of_program,command_list[2])
        label = memory[name_of_program][command_list[3]]
        pc=BRGT(var1,var2,label,pc,name_of_program)
        insert_to_mem(name_of_program,'pc',pc)
    elif(command_list[0]=='BRGE'):
        var1 = var_or_value(name_of_program,command_list[1])
        var2 = var_or_value(name_of_program,command_list[2])
        label = memory[name_of_program][command_list[3]]
        pc=BRGE(var1,var2,label,pc,name_of_program)
        insert_to_mem(name_of_program,'pc',pc)
    elif(command_list[0]=='BRLT'):
        var1 = var_or_value(name_of_program,command_list[1])
        var2 = var_or_value(name_of_program,command_list[2])
        label = memory[name_of_program][command_list[3]]
        pc=BRLT(var1,var2,label,pc,name_of_program)
        insert_to_mem(name_of_program,'pc',pc)
    elif(command_list[0]=='BRLE'):
        var1 = var_or_value(name_of_program,command_list[1])
        var2 = var_or_value(name_of_program,command_list[2])
        label = memory[name_of_program][command_list[3]]
        pc=BRLE(var1,var2,label,pc,name_of_program)
        insert_to_mem(name_of_program,'pc',pc)
    elif(command_list[0]=='BREQ'):
        var1 = var_or_value(name_of_program,command_list[1])
        var2 = var_or_value(name_of_program,command_list[2])
        label = memory[name_of_program][command_list[3]]
        BREQ(var1,var2,label,pc,name_of_program)
        insert_to_mem(name_of_program,'pc',pc)
    elif(command_list[0]=='BRA'):
        label = memory[name_of_program][command_list[1]]
        pc=BRA(label,name_of_program)
        insert_to_mem(name_of_program,'pc',pc)
    elif(command_list[0]=='RETURN'):
        RETURN(name_of_program)
        global program_terminate
        program_terminate=True
        


def main():

    global program_terminate
    while(1):
        if (program_terminate==False): 
            #read the code
            program = input()
 

            parser(program)
     
            p_id = name_of_programs.index(program)
            find_labels(name_of_programs[p_id])
            insert_to_mem(name_of_programs[p_id],'pc',0)

        while(program_terminate==False):
            program_counter = memory[name_of_programs[p_id]]['pc']
            run_command(name_of_programs[p_id],command[name_of_programs[p_id]][program_counter],program_counter)
            print(memory)

        program_terminate=False




global program_terminate
program_terminate = False
command = {}
name_of_programs= []
memory = {}
list_of_commands = ['LOAD','STORE','SET','ADD','SUB','MUL','DIV','MOD','BRGT',
        'BRGE','BRLT','BRLE','BREQ','BRA','DOWN','UP','SLEEP','PRINT','RETURN']
main()
