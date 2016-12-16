#Initialize memory !
memory = {}
global_memory = {}
list_of_commands = ['LOAD','STORE','SET','ADD','SUB','MUL','DIV','MOD','BRGT',
        'BRGE','BRLT','BRLE','BREQ','BRA','DOWN','UP','SLEEP','PRINT','RETURN']

command={}

"""This function is useful for print command
remove " from string and return a list with print command
string without " and Varname"""
def treat_print_statement(tmp_list):
    con_string = ''
    pointer1 = tmp_list.index('PRINT')+1
    pointer2 = len(tmp_list)
    for x in range(pointer1,pointer2-1):
        con_string = con_string +' '+ tmp_list[x]

    tmp_list1= tmp_list[0:tmp_list.index('PRINT')+1]
    tmp_list1.append(con_string[2:len(con_string)-1])
    tmp_list1.append(tmp_list.pop())
    return tmp_list1


#read all lines from .txt
def parser(program,p_id):
    global command

    fd = open(program,'r')

    is_program = fd.readline().strip('\n') 
    if (is_program!='#PROGRAM'):
        quit()
 
    #split each line of the program 
    tmp_list = []
    for line in fd.readlines():
        tmp_line = line.strip('\n').split()
        
        if ('PRINT' in tmp_line):
            tmp_line=treat_print_statement(tmp_line)

        if (tmp_line!=[]):
            tmp_list.append(tmp_line)
            command[p_id] = tmp_list



#check and saves to memory labels and their lines in .txt
def find_labels(name_of_program):
    global command

    #check if there is a label in commands!
    for i in range(0,len(command[name_of_program])):
        label = command[name_of_program][i][0]
        if label not in list_of_commands:
            insert_to_mem(name_of_program,label,i)

            #remove label from commands
            command[name_of_program][i].pop(0)

#Support array index.Save in memory like a variable.Like 'array[3]'
# not like 'array[i]' where i =3
def check_array_index(name_of_program,command_list):
    
    for item in command_list:
        if (command_list[0]=='PRINT' and command_list.index(item)==1):
            continue

        if ('[' in item):
            first_array = item[0:item.index('[')]
            index = item[item.index('[')+1:item.index(']')]
            index_value = str(var_or_value(name_of_program,index))
            new_index = first_array+'['+index_value+']'
            command_list[command_list.index(item)] = new_index
    
    return command_list

#insert variable and their values to memory
def insert_to_mem(name_of_program,key_var,value):
    global memory

    if (name_of_program in memory.keys()):
        memory[name_of_program][key_var] = value
    else:
        memory[name_of_program] = {key_var: value}

def insert_to_global_mem(key_var,value):
    global global_memory

    global_memory[key_var]=value

def read_from_global_mem(key_var):
    global global_memory
        
    value = global_memory[key_var]
    return value

#check if variable is in memory or raw value from .txt
def var_or_value(name_of_program,variable):
    global memory

    if (variable[0]=='$'):
        return int(memory[name_of_program][variable])
    else:
        return int(variable)

