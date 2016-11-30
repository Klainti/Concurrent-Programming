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


//csp_ctxt *cc;
buffer_args *arguments;

pthread_mutex_t monitor;

//function threads!
void *producer_thread(void *arg);
void *consumer_thread(void *arg);
void *buffer_thread(void *arg);

int main(int argc,char *argv[]){

    int i, bufSize, iret, len,producer_or_consumer,times;
    int *channel;
    int producer_channel=0;
    int consumer_channel;
    pthread_t tp,tc,tb;
    
    if (argc<4){
        debug_e("Give buffer_size and nof producers/consumer!!!");
        return(EXIT_FAILURE);
    }
    
    arguments = (buffer_args *) malloc (sizeof(buffer_args));
    if (arguments==NULL){
        debug_e("Malloc failed!");
        return(EXIT_FAILURE);
    }

    //inputs!
    bufSize = atoi(argv[1]);
    arguments->nOfProducers = atoi(argv[2]);
    arguments->nOfConsumers = atoi(argv[3]);
    consumer_channel = arguments->nOfProducers;
    len=arguments->nOfProducers + arguments->nOfConsumers;

    //initialize monitor and csp channels!
    pthread_mutex_init(&monitor,NULL);
    cc = (csp_ctxt *) malloc(sizeof(csp_ctxt)*len);
    if (cc==NULL){
        debug_e("Malloc failed!");
        return(EXIT_FAILURE);
    }
    csp_init(cc,len);

    iret = pthread_create(&tb,NULL,&buffer_thread, (void *) &bufSize);
    if (iret){
        debug_e("pthead_create error");
        return(EXIT_FAILURE);
    }

    for (i=0; i<len; i++){ //give channels to producers/consumers!
        channel = (int *) malloc(2*sizeof(int)); 
        if (channel==NULL){
            debug_e("Malloc failed!");
            return(EXIT_FAILURE);
        }

        scanf("%d %d", &producer_or_consumer,(channel+1));

        if (producer_or_consumer==0){
            *(channel+0) = producer_channel;
            producer_channel++;
     	    iret = pthread_create(&tp,NULL,&producer_thread, (void *) channel);
    	    if (iret){
        	    debug_e("pthead_create error");
        	    return(EXIT_FAILURE);
    	    }
        }
        else{
            *channel = consumer_channel;
            consumer_channel++;
     	    iret = pthread_create(&tc,NULL,&consumer_thread,(void *) channel);
    	    if (iret){
        	    debug_e("pthead_create error");
        	    return(EXIT_FAILURE);
    	    }
        }
    }


    pthread_join(tb,NULL);
      
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
    for (i = 0; i < len; i++){
        channels[i] = i;
        if (i < arguments->nOfProducers){
            debug("Produce channel: %d", i);
            channelsP[i] = i;
        }
        else{
            debug("Consumer channel: %d", i);
            channelsC[i - arguments->nOfProducers] = i;
        }
    }

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
    int *int_arg = (int *)arg;
    int channel,times,i;

    channel =int_arg[0];
    times =int_arg[1];

    for (i=0; i<times; i++){
        pthread_mutex_lock(&monitor);
        csp_send(cc,channel,&product);
        pthread_mutex_unlock(&monitor);
    }

    return NULL;
}

void *consumer_thread(void *arg){

    char get='G';
    char product;
    int *int_arg = (int *)arg;
    int channel,times,i;

    channel = int_arg[0];
    times = int_arg[1];

    for (i=0; i<times; i++){
        pthread_mutex_lock(&monitor);
        debug("Order a product from channel: %d",channel);
        csp_send(cc,channel,&get);
        pthread_mutex_unlock(&monitor);

        pthread_mutex_lock(&monitor);
        csp_recv(cc,channel,&product);
        debug("Get product %c  from channel: %d",product,channel);
        pthread_mutex_unlock(&monitor);
    }

    return NULL;

}
