#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#include "debug.h"

pthread_mutex_t mtx;
pthread_cond_t startTrain, embarkate, debarkate;
int nOfPassenger=0, N = 0, trainStarted = 0, trainFinished = 0;
int blockedEmbarkation = 0, blockedDebarkation = 0;

void *train();
void *passenger();
void trainStart();
void trainStop();
void embarkation();
void debarkation();

int main(int argc,char *argv[]){

    pthread_t train_thread,passenger_thread;
    int iret,i,total_passengers,timing;

    if (argc<3){
        debug_e("Give N and total number of passengers");
        return(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    total_passengers = atoi(argv[2]);

    debug("Init mtx at 1");
    pthread_mutex_init(&mtx, NULL);

    pthread_cond_init(&startTrain, NULL);
    pthread_cond_init(&embarkate, NULL);
    pthread_cond_init(&debarkate, NULL);

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
        debug("Enter monitor (trainStart)");
        pthread_mutex_lock(&mtx);
        trainStart();
        pthread_mutex_unlock(&mtx);
        debug("Exit monitor (trainStart)");
        debug("Train is about to begin");
        sleep(2);
        debug("Ride is over");
        debug("Enter monitor (trainStop)");
        pthread_mutex_lock(&mtx);
        trainStop();
        pthread_mutex_unlock(&mtx);
        debug("Exit monitor (trainStop)");
    }

    return NULL;
}

void *passenger(){
    debug("Enter monitor (embarkate)");
    pthread_mutex_lock(&mtx);
    debug("Passenger is going to embarkate");
    embarkation();
    pthread_mutex_unlock(&mtx);
    debug("Exit monitor (embarkate)");
    //Passenger wait for train to finish its ride to debarkate
    debug("Enter monitor (debarkate)");
    pthread_mutex_lock(&mtx);
    debug("Passenger is going to DEBARKATE");
    debarkation();
    pthread_mutex_unlock(&mtx);
    debug("Exit monitor (debarkate)");
    debug("Exit passenger's code");

    return NULL;
};

void trainStart(){
    debug("trainStarted = %d || trainFinished = %d", trainStarted, trainFinished);
    if(trainStarted == 0 || trainFinished == 1){
        debug("Train waits to full");
        pthread_cond_wait(&startTrain, &mtx);
    }
}

void trainStop(){
    trainFinished = 1;
    debug("Debarkation is about to begin");
    pthread_cond_signal(&debarkate);
}

void embarkation(){
    if(trainStarted == 1){
        /*Train is started.
        Other passengers must wait until train finish its ride. */
        blockedEmbarkation++;
        debug("Passenger waits for train to finish its ride");
        pthread_cond_wait(&embarkate, &mtx);
    }
    nOfPassenger++;
    debug("Passenger: %d",nOfPassenger);
    if(nOfPassenger == N){
        /*Train is full.
        Notify train to start the ride */
        debug("Last passenger,start the train ");
        trainStarted = 1;
        pthread_cond_signal(&startTrain);
    }
    else{
        if(blockedEmbarkation != 0){
            blockedEmbarkation--;
            debug("Passenger wakes up other blocked passenger");
            pthread_cond_signal(&embarkate);
        }
    }
}

void debarkation(){
    debug("Passenger wait for train to finish its ride %d",nOfPassenger);
    if(trainFinished == 0){
        blockedDebarkation++;
        debug("Passenger waits for train to finish, to debarkate");
        pthread_cond_wait(&debarkate, &mtx);
    }
    nOfPassenger--;
    debug("Passenger debarkated.Remaining %d", nOfPassenger);
    if (nOfPassenger == 0){
        /*Last passenger out of the train.
        Notify other passenger to embarkate */
        debug("Last passenger,out of train");
        trainStarted = 0;
        if (blockedEmbarkation != 0){
            blockedEmbarkation--;
            trainFinished = 0;
            pthread_cond_signal(&embarkate);
        }
    }
    else{
        if (blockedDebarkation != 0) {
            blockedDebarkation--;
            pthread_cond_signal(&debarkate);
        }
    }
}
