#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc,char *argv[]){
    
    int fd[2],write_byte,read_byte;

    pipe(fd);

    write_byte = getchar();
    while(write_byte!=EOF){
        write(fd[1],&write_byte,sizeof(char));
        read(fd[0],&read_byte,sizeof(char));
        printf("%c",read_byte);
        write_byte = getchar();
    }

    return(0);
}
