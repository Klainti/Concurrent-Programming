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




def bgrt(var1,var2,label,pc):
    
    if (var1>var2):
        return label
    else:
        return pc+1

    return None

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

def run_command(name_of_program,command_list):
    
    if (command_list[0]=='ADD'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        ADD(var_name,var1,var2,name_of_program)
    elif (command_list[0]=='SUB'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        SUB(var_name,var1,var2,name_of_program)
    elif (command_list[0]=='MUL'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        MUL(var_name,var1,var2,name_of_program)
    elif (command_list[0]=='DIV'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        DIV(var_name,var1,var2,name_of_program)
    elif (command_list[0]=='MOD'):
        var_name = command_list[1]
        var1= var_or_value(name_of_program,command_list[2])
        var2 = var_or_value(name_of_program,command_list[3])
        MOD(var_name,var1,var2,name_of_program)
    elif(command_list[0]=='SET'):
        var_name= command_list[1]
        var = var_or_value(name_of_program,command_list[2])
        SET(var_name,var,name_of_program)



def main():
    #read the code
    program = input()
 

    parser(program)
     
    p_id = name_of_programs.index(program)
    find_labels(name_of_programs[p_id])

    for i in range(5):
        run_command(name_of_programs[p_id],command[name_of_programs[p_id]][i])
        print(memory)




command = {}
name_of_programs= []
memory = {}
list_of_commands = ['LOAD','STORE','SET','ADD','SUB','MUL','DIV','MOD','BRGT',
        'BRGE','BRLT','BRLE','BREQ','BRA','DOWN','UP','SLEEP','PRINT','RETURN']



"""
for _ in range(1):
    p_id = name_of_programs.index(program)
    
    pc=0
    for j in range(5):
        if (command[name_of_programs[p_id]][pc][0]=='ADD'):
            var_name = command[name_of_programs[p_id]][pc][1]
            var1 = int(command[name_of_programs[p_id]][pc][2])
            var2 = int(command[name_of_programs[p_id]][pc][3])
            add(var_name,var1,var2,name_of_programs[p_id])
            pc +=1
            print(pc)
        else:
            label = int(command[name_of_programs[p_id]][pc][3])
            print(label)

            var1 = command[name_of_programs[p_id]][pc][1]
            print(var1)
            if ('$' in var1):
                var1 = memory[name_of_programs[p_id]][var1[1:]]
            else:
                var1 = int(var1)
 
            var2 = command[name_of_programs[p_id]][pc][2]
            print(var2)
            if ('$' in var2):
                var2 = memory[name_of_programs[p_id]][var2[1:]]
            else:
                var2 = int(var2)

            print(var1)
            print(var2)

            pc = bgrt(var1,var2,label,pc)
        
            print(pc)
"""
main()
