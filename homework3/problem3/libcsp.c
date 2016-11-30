#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "libcsp.h"
#include "debug.h"
#define NOT_CHANNEL -1

csp_ctxt *cc;
pthread_mutex_t monitor;

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
        for (i=0; i<cc[chan].waiting_chans_len; i++){
            if (cc[chan].waiting_chans[i]!=chan){
                cc[cc[chan].waiting_chans[i]].waiting_chans=NULL;
                cc[cc[chan].waiting_chans[i]].send_cond=NULL;
            }
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
    
    debug("Receive a msg from channel: %d",chan);
    
    if (cc[chan].sent==false){
        csp_wait(cc,&chan,1);
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
