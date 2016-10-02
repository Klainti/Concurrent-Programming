#include <stdio.h>
#include <pthread.h>
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


    printf("pthread_create() for thread_read return %d\n",read_iret);
    printf("pthread_create() for thread_write return %d\n",read_iret);


    //wait for thread to finish
    while(!thread_read_done){};
    while(!thread_write_done){};

    pipe_close();
    return(0);
}

//initialize pipe
void pipe_init(int size){
    
    pipe = (char *)calloc(size,sizeof(char));
    read_or_write = (bool *)calloc(size,sizeof(bool));
}

//close pipe and free memory
void pipe_close(){
    
    read_in=-1; // indicator that pipe is closed
    free((void*) pipe);
    free((void*) read_or_write);
}

void pipe_write( char write_byte) {

    //wait until the slot for write is available
    while (*(read_or_write+write_out)!=false) {
    }
    
   
    //free slot,write!
    pipe[write_out] = write_byte ;
    *(read_or_write+write_out)=true;
  
    //increment write_out
    write_out = ((write_out+1) % size_of_pipe);
}

int pipe_read(char *read_byte) {
    
    // pipe is closed
    if (read_in == -1 ) {
       return 0;
    }
    
    //wait to write a byte
    while (*(read_or_write+read_in)!=true ) {
        if (read_in == -1 ) {
           return 0;
        }
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
        printf("%c\n",character);
    }
    thread_read_done = 1;
}

void *thread_write(){

    int i;
    
    for (i=0; i<5; i++){
        pipe_write('a'+i);
    }
    pipe_close();
    thread_write_done=1;
}
