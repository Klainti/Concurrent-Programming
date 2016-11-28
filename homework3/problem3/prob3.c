#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "debug.h"
#include "libcsp.h"

//#define NOT_CHANNEL -1

typedef struct{
	int nOfProducers;
	int nOfConsumers;
}buffer_args;

/*
typedef struct {

    //condition for send and receive a msg
    pthread_cond_t *send_cond;
    pthread_cond_t recv_cond;

    //checks for a msg 
    bool sent;
    bool recv;

    //all channels waiting in csp_wait!
    int *waiting_chans;
    int waiting_chans_len;

    //msg
    char *data;

}csp_ctxt;
*/

//csp_ctxt *cc;
buffer_args *arguments;

pthread_mutex_t monitor;

/*
//initialize frame sync for a specific number of channels.
void csp_init(csp_ctxt *cc,int nofchans);

//Send a msg to chan
int csp_send(csp_ctxt *cc, int chan, char *msg);

//Receive a msg from chan
int csp_recv(csp_ctxt *cc,int chan,char *msg);

//Wait till a channel send msg.Return the channel 
int csp_wait(csp_ctxt *cc, int chans[], int len);

*/

//function threads!
void *producer_thread(void *arg);
void *consumer_thread(void *arg);
void *buffer_thread(void *arg);

int main(int argc,char *argv[]){

    int i, bufSize, iret, len;
    int *channel;
    pthread_t tp,tc,tb;
    
    
    arguments = (buffer_args *) malloc (sizeof(buffer_args));
    if (arguments==NULL){
        debug_e("Malloc failed!");
        return(EXIT_FAILURE);
    }

    printf("Enter the number of producers and consumers: ");
    scanf("%d %d", &(arguments->nOfProducers), &(arguments->nOfConsumers));
    len=arguments->nOfProducers + arguments->nOfConsumers;

    printf("Enter the size of the buffer: ");
    scanf("%d", &bufSize);

    debug_e("%d %d %d",arguments->nOfProducers,arguments->nOfConsumers,bufSize);

    channel = (int *) malloc(sizeof(int)*len);
    //initialize monitor and csp channels!
    pthread_mutex_init(&monitor,NULL);
    cc = (csp_ctxt *) malloc(sizeof(csp_ctxt)*len);
    if (cc==NULL){
        debug_e("Malloc failed!");
        return(EXIT_FAILURE);
    }
    csp_init(cc,len);

    debug_e("Create buffer thread");
    iret = pthread_create(&tb,NULL,&buffer_thread, (void *) &bufSize);
    if (iret){
        debug_e("pthead_create error");
        return(EXIT_FAILURE);
    }
    debug_e("Create producers");

    for (i = 0; i < arguments->nOfProducers; i++){
	    channel[i] = i; 
    	iret = pthread_create(&tp,NULL,&producer_thread, (void *) &channel[i]);
    	if (iret){
        	debug_e("pthead_create error");
        	return(EXIT_FAILURE);
    	}
    }
    debug_e("Create consumer");

    for (i = 0; i < arguments->nOfConsumers; i++){
	    channel[arguments->nOfProducers + i] = arguments->nOfProducers + i; 
    	iret = pthread_create(&tc,NULL,&consumer_thread,(void *) &channel[arguments->nOfProducers + i]);
    	if (iret){
        	debug_e("pthead_create error");
        	return(EXIT_FAILURE);
    	}
    }

    while(1);
      
    return(0);
}

void *buffer_thread(void *arg){
	int i, n = 0, in = 0, out = 0, channel;
	int bufSize =  *(int *) arg;
    int len = arguments->nOfProducers + arguments->nOfConsumers;
	char request;
	int channels[len];
	int channelsP[arguments->nOfProducers];
	int channelsC[arguments->nOfConsumers];
    char data = 'D';


    //init arrays with channels for consumer,producer
    debug_e("Create arrays");
    for (i = 0; i < len; i++){
        channels[i] = i;
        if (i < arguments->nOfProducers){
            channelsP[i] = i;
        }
        else{
            channelsC[i - arguments->nOfProducers] = i;
        }
    }

    debug_e("Check produce/consume");
	while(1){
		if (n > 0 && n < bufSize){ //consume or produce
            debug("Producers and consumers");
			pthread_mutex_lock(&monitor);
    		channel = csp_wait(cc, channels, len);
    		pthread_mutex_unlock(&monitor);

    		pthread_mutex_lock(&monitor);
         	csp_recv(cc,channel,&request);
    	    if (request == 'P'){
    	    	//Producer
                debug("Channel: %d Request: %c", channel,request);
    	    	in = (in + 1) % bufSize;
    	    	n++;
                debug("Buffer--> Capacity: %d and in_data: %d", n,in);
    	    }
    	    else if (request == 'G'){
    	    	//Consumer
                debug("Channel: %d Request: %c", channel,request);
    	    	out = (out + 1) % bufSize;
    	    	n--;
                debug("Buffer--> Capacity: %d and out_data: %d", n,out);
                csp_send(cc,channel,&data);

    	    }
            pthread_mutex_unlock(&monitor);
    	}
    	else if (n == 0){
    		//buffer is empty
    		//Only producers are allowed
            debug("Only producers");
            pthread_mutex_lock(&monitor);
    		channel = csp_wait(cc, channelsP, arguments->nOfProducers);
            csp_recv(cc,channel,&request);
            debug("Channel: %d Request: %c", channel,request);
    		in = (in + 1) % bufSize;
    		n++;
            debug("Buffer--> Capacity: %d and in_data: %d", n,out);
            pthread_mutex_unlock(&monitor);
    	}
    	else{
    		//Buffer is full
    		//Only consumers are allowed
            debug("Only consumers");
            pthread_mutex_lock(&monitor);
    		channel = csp_wait(cc, channelsC, arguments->nOfConsumers);
            csp_recv(cc,channel,&request);
            debug("Channel: %d Request: %c", channel,request);
    		out = (out + 1) & bufSize;
    		n--;
            debug("Buffer--> Capacity: %d and out_data: %d", n,out);
            csp_send(cc,channel,&data);
            pthread_mutex_unlock(&monitor);
    	}
	}
}

void *producer_thread(void *arg){
    
    char product ='P';
    int channel =*(int *)arg;

    pthread_mutex_lock(&monitor);
    csp_send(cc,channel,&product);
    pthread_mutex_unlock(&monitor);

    return NULL;
}

void *consumer_thread(void *arg){

    char get='G';
    char product;
    int channel =*(int *)arg;

    pthread_mutex_lock(&monitor);
    debug("Order a product from channel: %d",channel);
    csp_send(cc,channel,&get);
    pthread_mutex_unlock(&monitor);

    pthread_mutex_lock(&monitor);
    csp_recv(cc,channel,&product);
    debug("Get product %c  from channel: %d",product,channel);
    pthread_mutex_unlock(&monitor);
    

    return NULL;

}

/*

void csp_init(csp_ctxt *cc,int nofchans){

    int i;
    debug("Begin csp_init!");
    
    for (i=0; i<nofchans; i++){
        pthread_cond_init(&cc[i].recv_cond,NULL);
        cc[i].send_cond=NULL;
        cc[i].sent = false;
        cc[i].recv = false;
        cc[i].data = NULL;
        cc[i].waiting_chans = NULL;
    }
    debug("Done csp_init");
}

int csp_send(csp_ctxt *cc, int chan, char *msg){

    int i;

    debug("Send a msg from channel: %d", chan);
    cc[chan].recv = false;
    cc[chan].data = msg;
    cc[chan].sent = true;
        
    //notify channel!
    if (cc[chan].send_cond!=NULL){
        debug("len----> %d", cc[chan].waiting_chans_len);
        debug("Inside NULL statements"); 
        for (i=0; i<cc[chan].waiting_chans_len; i++){
            debug_e("Inside Null: --> %d", cc[chan].waiting_chans[i]);
            if (cc[chan].waiting_chans[i]!=chan){
                cc[cc[chan].waiting_chans[i]].waiting_chans=NULL;
                cc[cc[chan].waiting_chans[i]].send_cond=NULL;
            }
        }

        for (i=0; i<cc[chan].waiting_chans_len; i++){
            debug("After nulling: %d : %d", cc[chan].waiting_chans[i],cc[cc[chan].waiting_chans[i]].send_cond);
        }

        pthread_cond_signal(cc[chan].send_cond);
    }

    //wait for reply
    if (cc[chan].recv==false){
        debug("Wait for reply at channel: %d",chan);
        pthread_cond_wait(&cc[chan].recv_cond, &monitor);
    }

    debug("Done send at channel: %d", chan);

    return(1);
}

int csp_recv(csp_ctxt *cc,int chan,char *msg){
    
    debug("Receiver waits for a msg from channel: %d",chan);

    csp_wait(cc,&chan,1);
    //there is a msg at channel,receive it!
    cc[chan].sent = false;
    *msg=*(cc[chan].data);
    cc[chan].recv = true;

    //notify sender
    pthread_cond_signal(&cc[chan].recv_cond);

    debug("Done receive at channel: %d",chan);

    return(1);
}

int csp_wait(csp_ctxt *cc, int chans[], int len){
    
    int i;
    pthread_cond_t tmp_wait;
    pthread_cond_init(&tmp_wait,NULL);

    debug("csp_wait check for a msg!");
    //check for a msg
    for (i=0; i<len; i++){
        if (cc[chans[i]].sent==true){
            debug("csp_wait return channel: %d",chans[i]);
            cc[chans[i]].send_cond=NULL;
            return chans[i];
        }
    }

    //assign cond for each channel in chans[]
    for (i=0; i<len; i++){
        cc[chans[i]].send_cond = &tmp_wait;
        debug_e("chan: %d ---> %d",chans[i],cc[chans[i]].send_cond);
        cc[chans[i]].waiting_chans = chans;
        cc[chans[i]].waiting_chans_len = len;

    }

    debug("csp_wait: wait at condition");
    //wait for a msg
    pthread_cond_wait(&tmp_wait,&monitor);
    
    //check for a msg
    for (i=0; i<len; i++){
        if (cc[chans[i]].sent==true){
            cc[chans[i]].waiting_chans=NULL;
            cc[chans[i]].send_cond=NULL;
            debug("csp_wait wake up from channel: %d",i);
            return chans[i];
        }
    }
    
    return(NOT_CHANNEL);
}

*/
