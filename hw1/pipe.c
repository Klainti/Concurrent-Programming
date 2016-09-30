#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
/************Shared memory*********/
volatile int read_in=0;  //read_in for writing,
volatile int write_out=0; //write_out for reading
volatile char *pipe; 
int size_of_pipe;
/**********************************/

/**********Functions***************/

//initialize pipe
void pipe_init(int size){
    
    pipe = (char *)calloc(size,sizeof(char));
    
}

//close pipe and free memory
void pipe_close(){
    
    read_in=-1; // indicator that pipe is closed
    free((void*) pipe);
}

/***********************************/


void pipe_write( char write_byte) {

    //grapse an uparxei xwros
    while ( (write_out-read_in) == -1) {
        //oso den uparxei xwros, perimene
        // oso h diafora einai 1, den mporoume naw grapsoume
    }
    
    /* an o agwgos einai closed, tote h write apotugxanei
     * if ( read_in == -1 ) {
        printf("error\n");
        exit(0);
    }
    */
    
    //eleftherothike xwros, ara grapse
    pipe[write_out] = write_byte ;
    loop = write_out == size_of_pipe-1 ? true : false;

    //increment ton pointer
    write_out = (write_out+1) % size_of_pipe;
    
    /*write_out = write_out++;//auksise 
    if ( write_out == size) {
        write_out = 0 ; //Kaname ena 'loop', ara twra eimaste kai pali sthn prwti thesi tou pinaka
    }*/
}

int pipe_read(char *read_byte) {
    
    // an o agwgos einai closed, tote h read epistrefei 0  
    if (read_in == -1 ) {
       return 0;
    }
    
    //oso h diafora tous einai 0 , tote den diavazw, perimenw
    while ( abs(write_out - read_in) == 0 ) {
        
    }
    
    //alliws mporei na diavasei
    *read_byte = *(pipe+read_in);
    
    //auksanoume kata 1 tin thesi
    read_in = (read_in+1) % size_of_pipe;
    
    return (1); 
    
}

int main(int argc,char *argv[]){

    int i;
    char razoras;
    //check for right paremeters
    if (argc<3){
        printf("Give K and N!\n");
        return(0);
    }

    size_of_pipe = atoi(argv[1]);

    //first create pipe
    pipe_init(size_of_pipe);

    /*//check if it works
    for (i=0; i<5; i++){
        pipe_write('a'+i);
        if (pipe_read(&razoras)){
            printf("%c\n",razoras);
        }
        else{
            printf("Nope,debug it!\n");
        }
    }
    */

    pipe_write('a');
    pipe_write('b');
    pipe_read(&razoras);
    pipe_write('c');
    pipe_write('d');
    pipe_write('e');
    pipe_write('f');
    pipe_write('g');

    pipe_close();
    return(0);
}
