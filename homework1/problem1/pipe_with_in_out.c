#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <stdbool.h>

#define CLOSED_PIPE = 2;

/************Shared memory*********/
volatile int in = -1;  //read_in for writing,
volatile int out = 0; //write_out for reading
volatile int pipe_is_closed = 0;
volatile char *pipe;
int size_of_pipe;

//finish indicators for threads
volatile int thread_read_done=0;
volatile int thread_write_done=0;
/**********************************/

/**********Functions***************/
void pipe_init(int size);
void pipe_close();
void pipe_write(char write_byte);
int  pipe_read(char *read_byte);

void *thread_read();
void *thread_write();
/*********************************/

int main(int argc,char *argv[]){

    pthread_t thread1,thread2;
    int read_iret,write_iret;


    //check for paremeters
    if (argc<3){
        printf("Give K and N!\n");
        return(0);
    }

    size_of_pipe = atoi(argv[1]);

    //first create pipe
    pipe_init(size_of_pipe);

    //Create the reading thread
    read_iret= pthread_create(&thread1,NULL,thread_read,NULL);
    if (read_iret){
        fprintf(stderr,"Error - thread1() return code: %d\n",read_iret);
        exit(EXIT_FAILURE);
    }

    //Create the writing thread
    write_iret = pthread_create(&thread2,NULL,thread_write,NULL);
    if (write_iret){
        fprintf(stderr,"Error - thread2 return code: %d\n",write_iret);
        exit(EXIT_FAILURE);
    }


    //printf("pthread_create() for thread_read return %d\n",read_iret);
    //printf("pthread_create() for thread_write return %d\n",read_iret);


    //wait for thread to finish
    while(!thread_read_done){
        sched_yield();
    }
    while(!thread_write_done){
        sched_yield();
    }

    free((void*) pipe);
    return(0);
}

//initialize pipe
void pipe_init(int size){
    pipe = (char *)calloc(size,sizeof(char));
}

//close pipe and free memory
void pipe_close(){
    pipe_is_closed = 1; // indicator that pipe is closed
}

void pipe_write(char write_byte) {

  while (!((((in+1)%size_of_pipe) != (out%size_of_pipe)) || in == (-1))){
    //sched_yield();
  }
  in++;
  pipe[in%size_of_pipe] = write_byte;
}

int pipe_read(char *read_byte) {

  if (pipe_is_closed == 1){
    return 0;
  }

  while (!(out <= in)){
    if (pipe_is_closed == 1){
      return 0;
    }
    sched_yield();
  }
  *read_byte = pipe[out%size_of_pipe];
  out++;
  return(1);
    // // pipe is closed
    // //if (write_close == 1 ) {
    //   // return 0;
    // //}
    //
    // //wait to write a byte
    // while (*(read_or_write+read_in)!=true ) {
    //     if (write_close == 1 ) {
    //        return 0;
    //     }
    //     sched_yield();
    // }
    //
    // //read next byte
    // *read_byte = *(pipe+read_in);
    // *(read_or_write+read_in)=false;
    //
    // //increment read in by 1
    // read_in = ((read_in+1) % size_of_pipe);
    //
    // return (1);

}

void *thread_read(){

    char charRead;
    while(pipe_read(&charRead)){
        printf("%c",charRead);
    }
    thread_read_done = 1;
    return NULL;
}

void *thread_write(){

    char charWrite;
    charWrite = getchar();
    while (charWrite!=EOF){
        pipe_write(charWrite);
        charWrite = getchar();
    }
    pipe_close();
    thread_write_done=1;
    return NULL;
}
