#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#include "debug.h"

pthread_mutex_t start_train,debarkation,embarkation;
int nOfPassenger=0;
int N;


void *train();
void *passenger();

int main(int argc,char *argv[]){

    pthread_t train_thread,passenger_thread;
    int iret,i,total_passengers,timing;

    if (argc<3){
        debug_e("Give N and total number of passengers");
        return(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    total_passengers = atoi(argv[2]);

    debug("Init start_train at 0");
    pthread_mutex_init(&start_train, NULL);
    pthread_mutex_lock(&start_train);
    
    debug("Init debarkation at 0");
    pthread_mutex_init(&debarkation, NULL);
    pthread_mutex_lock(&debarkation);

    iret = pthread_create(&train_thread,NULL,&train,NULL);
    if (iret){
        debug_e("pthread_create for train fail");
        return(EXIT_FAILURE);
    }

    for (i=0; i<total_passengers; i++){
        scanf("%d",&timing);
        sleep(timing);
        iret = pthread_create(&passenger_thread,NULL,&passenger,NULL);
        if (iret){
            debug_e("pthread_create for train fail");
            return(EXIT_FAILURE);
        }
    }

    pthread_join(train_thread,NULL);

    return (0);
}


void *train(){

    while(1){
        debug("Locking train");
        pthread_mutex_lock(&start_train);
        debug("Train is about to begin");
        sleep(2);
        debug("Ride is over");
        pthread_mutex_unlock(&debarkation);
    }

    return NULL;
}

void *passenger(){

    pthread_mutex_lock(&embarkation);
    nOfPassenger ++;
    debug("Passenger: %d",nOfPassenger);
    if (nOfPassenger==N){
        debug("Last passenger,start the train ");
        pthread_mutex_unlock(&start_train);
    }
    else{
        debug("Train is not full");
        pthread_mutex_unlock(&embarkation);
    }

    debug("Passenger wait for train to finish %d",nOfPassenger);
    pthread_mutex_lock(&debarkation);
    nOfPassenger--;
    debug("Passenger debarkate.Remaining %d", nOfPassenger);
    if (nOfPassenger==0){
        debug("Last passenger,out of train");
        pthread_mutex_unlock(&embarkation);
    }
    else{
        debug("Notify next passenger to debarkate");
        pthread_mutex_unlock(&debarkation);
    }

    return NULL;
};
