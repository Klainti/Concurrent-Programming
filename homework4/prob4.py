def add(var_name,var1,var2,name_of_program):
    if (name_of_program in memory.keys()):
        memory[name_of_program][var_name] = var1+var2
    else:
        memory[name_of_program] = {var_name: var1+var2}
    return None

def bgrt(var1,var2,label,pc):
    
    if (var1>var2):
        return label
    else:
        return pc+1


command = {}
name_of_programs= []
memory = {}
for _ in range(1):
    #read the code
    program = input()
    fd = open(program,'r')
    name_of_programs.append(fd.readline().strip('\n'))
    p_id = name_of_programs.index(program)
    
    #split each line of the program 
    tmp_list = []
    for line in fd.readlines():
        tmp_list.append(line.strip('\n').split())
        command[name_of_programs[p_id]] = tmp_list

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

print(memory)
