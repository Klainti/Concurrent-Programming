#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <stdbool.h>

/************Shared memory*********/
volatile int read_in=0;  //read_in for writing,
volatile int write_out=0; //write_out for reading
volatile char *pipe; 
volatile bool *read_or_write;
int size_of_pipe;

//finish indicators for threads
volatile int thread_read_done=0; 
volatile int thread_write_done=0;
volatile int write_close=0;
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

    read_iret= pthread_create(&thread1,NULL,thread_read,NULL);
    if (read_iret){
        fprintf(stderr,"Error - thread1() return code: %d\n",read_iret);
        exit(EXIT_FAILURE);
    } 

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
    free((void*) read_or_write);
    return(0);
}

//initialize pipe
void pipe_init(int size){
    
    pipe = (char *)calloc(size,sizeof(char));
    read_or_write = (bool *)calloc(size,sizeof(bool));
}

//close pipe and free memory
void pipe_close(){    
    write_close = 1; // indicator that pipe is closed
}

void pipe_write( char write_byte) {

    //wait until the slot for write is available
    while (*(read_or_write+write_out)!=false) {
        sched_yield();
    }
    
   
    //free slot,write!
    pipe[write_out] = write_byte ;
    *(read_or_write+write_out)=true;
  
    //increment write_out
    write_out = ((write_out+1) % size_of_pipe);
}

int pipe_read(char *read_byte) {
    
    // pipe is closed
    //if (write_close == 1 ) {
      // return 0;
    //}
    
    //wait to write a byte
    while (*(read_or_write+read_in)!=true ) {
        if (write_close == 1 ) {
           return 0;
        }
        sched_yield();
    }
    
    //read next byte 
    *read_byte = *(pipe+read_in);
    *(read_or_write+read_in)=false;

    //increment read in by 1
    read_in = ((read_in+1) % size_of_pipe);
    
    return (1); 
    
}

void *thread_read(){

    char character;
    while(pipe_read(&character)){
        printf("%c",character);
    }
    thread_read_done = 1;
    return NULL;
}

void *thread_write(){
    
    char byte_from_file;
    byte_from_file = getchar();
    while (byte_from_file!=EOF){
        pipe_write(byte_from_file);
        byte_from_file = getchar();
    }
    pipe_close();
    thread_write_done=1;
    return NULL;
}
