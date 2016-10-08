#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdbool.h>

int fd[2];
bool read_done=false;
bool write_done=false;

void *read_pipe(void *ptr){
    
    char read_byte;

    while(read(fd[0],&read_byte,sizeof(char))){
        printf("%c", read_byte);
    }

    close(fd[0]);
    read_done=true;
    return NULL;
}

void *write_pipe(void *ptr){

    char write_byte;

    write_byte = getchar();
    while (write_byte!=EOF){
        write(fd[1],&write_byte,sizeof(char));
        write_byte = getchar();
    }

    close(fd[1]);
    write_done=true;
    return NULL;
}


int main(int argc,char *argv[]){

    pthread_t thread1,thread2;
    int iret1,iret2;
    
    pipe(fd);

    iret1 = pthread_create(&thread1,NULL,read_pipe,NULL);
    if (iret1==1){
        fprintf(stderr,"Error - pthread_create() return: %d\n",iret1);
        return(EXIT_FAILURE);
    }

    iret2 = pthread_create(&thread2,NULL,write_pipe,NULL);
    if (iret2==1){
        fprintf(stderr,"Error - pthread_create() return: %d\n",iret2);
        return(EXIT_FAILURE);
    }

    while(!read_done){sched_yield();};
    while(!read_done){sched_yield();};
    return(0);
}
