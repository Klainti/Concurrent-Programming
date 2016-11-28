#ifndef __LIBCSP_H__
#define __LIBCSP_H__


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

extern csp_ctxt *cc;

//initialize frame sync for a specific number of channels.
void csp_init(csp_ctxt *cc,int nofchans);

//initialize frame sync for a specific number of channels.
void csp_init(csp_ctxt *cc,int nofchans);

//Send a msg to chan
int csp_send(csp_ctxt *cc, int chan, char *msg);

//Receive a msg from chan
int csp_recv(csp_ctxt *cc,int chan,char *msg);

//Wait till a channel send msg.Return the channel 
int csp_wait(csp_ctxt *cc, int chans[], int len);

#endif // __LIBCSP_H__
