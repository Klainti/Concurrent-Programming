#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "debug.h"

typedef struct {

    //condition for send and receive a msg
    pthread_cond_t send_cond;
    pthread_cond_t recv_cond;

    //checks for a msg 
    bool sent;
    bool recv;

    //msg
    char *data;

}csp_ctxt;

csp_ctxt *cc;

pthread_mutex_t monitor;


//initialize frame sync for a specific number of channels.
void csp_init(csp_ctxt *cc,int nofchans);

//Send a msg to chan
int csp_send(csp_ctxt *cc, int chan, char *msg);

//Receive a msg from chan
int csp_recv(csp_ctxt *cc,int chan,char *msg);

//function threads!
void *sender_thread(void *arg);
void *receiver_thread(void *arg);

int main(int argc,char *argv[]){

    int iret;
    pthread_t t1,t2;

    //initialize monitor and csp channels!
    pthread_mutex_init(&monitor,NULL);
    cc = (csp_ctxt *) malloc(1*sizeof(csp_ctxt));
    if (cc==NULL){debug_e("Calloc failed!");}
    csp_init(cc,1);

    iret = pthread_create(&t1,NULL,&sender_thread,NULL);
    if (iret){
        debug_e("pthead_create error");
        return(EXIT_FAILURE);
    }

    iret = pthread_create(&t2,NULL,&receiver_thread,NULL);
    if (iret){
        debug_e("pthead_create error");
        return(EXIT_FAILURE);
    }

    while(1);
      
    return(0);
}

void *sender_thread(void *arg){
    
    char text ='F';

   // while(1){
        pthread_mutex_lock(&monitor);
        csp_send(cc,0,&text);
        pthread_mutex_unlock(&monitor);
   // }
    return NULL;
}

void *receiver_thread(void *arg){

    char msg;

   //while(1){
        pthread_mutex_lock(&monitor);
        csp_recv(cc,0,&msg);
        debug("Message received----> %c",msg);
        pthread_mutex_unlock(&monitor);
    //}
    return NULL;

}

void csp_init(csp_ctxt *cc,int nofchans){

    int i;
    debug("Begin csp_init!");
    
    for (i=0; i<nofchans; i++){
        pthread_cond_init(&cc[i].recv_cond,NULL);
        pthread_cond_init(&cc[i].send_cond,NULL);
        cc[i].sent = false;
        cc[i].recv = false;
        cc[i].data = NULL;
    }
    debug("Done csp_init");
}

int csp_send(csp_ctxt *cc, int chan, char *msg){

    debug("Send a msg from channel: %d", chan);
    cc[chan].recv = false;
    cc[chan].data = msg;
    cc[chan].sent = true;
    
    //notify receiver!
    pthread_cond_signal(&cc[chan].send_cond);

    //wait for reply
    if (cc[chan].recv==false){
        debug("Wait for reply at channel: %d",chan);
        pthread_cond_wait(&cc[chan].recv_cond, &monitor);
    }

    debug("Done send at channel: %d", chan);

    return(1);
}

int csp_recv(csp_ctxt *cc,int chan,char *msg){
    
    debug("Receive a msg from channel: %d",chan);

    //check for a msg at channel
    if (cc[chan].sent==false){
        debug("No msg at channel: %d ", chan);
        pthread_cond_wait(&cc[chan].send_cond,&monitor);
    }

    //there is a msg at channel,receive it!
    cc[chan].sent = false;
    *msg=*(cc[chan].data);
    cc[chan].recv = true;

    //notify sender
    pthread_cond_signal(&cc[chan].recv_cond);

    debug("Done receive at channel: %d",chan);

    return(1);
}
