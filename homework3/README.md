# Assignment

3.1 Protocol for the traffic control on a narrow bridge ensuring
 -Same as 2.2 but with monitors !
 
 3.2 Roller-coaster
  -Same as 2.3 but with monitors !
  
  3.3 API for CSP
  
    typedef struct {...} csp ctxt; --> Characterize the state of a channel.
    
    void csp_init(csp_ctxt **cc, int nofchans) --> Initialize each channel
    
    int csp_send(csp_ctxt *cc, int chan, char *msg) --> Send a msg over a channel (*cc)
    
    int csp_recv(csp_ctxt *cc, int chan, char *msg) --> Receive a msg over a channel
    
    int csp_wait(csp *cc, int chans[], int len) --> Concurrent waiting over chans[], till a msg received
    
    Implement a producers-consumers algorithm over CSP
    
Programming: C, using pthreads
