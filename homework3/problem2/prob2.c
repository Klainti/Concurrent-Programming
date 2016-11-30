#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#include "debug.h"

pthread_mutex_t mtx, debug_mtx;
pthread_cond_t startTrain, embarkate, debarkate;
int nOfPassenger=0, N = 0, trainStarted = 0, trainFinished = 0;
int blockedEmbarkation = 0, blockedDebarkation = 0;

void *train();
void *passenger(void *arg);
void trainStart();
void trainStop();
void embarkation(int id);
void debarkation(int id);

int main(int argc,char *argv[]){

    pthread_t train_thread,passenger_thread;
    int iret,i,total_passengers,timing;
    int *id;

    if (argc<3){
        debug_e("Give N and total number of passengers");
        return(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    total_passengers = atoi(argv[2]);

    id = (int *) malloc(total_passengers * sizeof(int));

    debug("Init mtx at 1");
    pthread_mutex_init(&mtx, NULL);
    pthread_mutex_init(&debug_mtx, NULL);

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
        id[i] = i;
        iret = pthread_create(&passenger_thread,NULL,&passenger, (void *) (id + i));
        if (iret){
            debug_e("pthread_create for train fail");
            return(EXIT_FAILURE);
        }
    }

    pthread_join(train_thread,NULL);

    free(id);

    return (0);
}


void *train(){

    while(1){
    	pthread_mutex_lock(&debug_mtx);
        debug("Train is entering monitor to wait to be full");
        pthread_mutex_unlock(&debug_mtx);
        pthread_mutex_lock(&mtx);
        trainStart();
        pthread_mutex_lock(&debug_mtx);
        debug("Train exits monitor");
        pthread_mutex_unlock(&debug_mtx);
        pthread_mutex_unlock(&mtx);
        pthread_mutex_lock(&debug_mtx);
        debug("Train is about to begin");
        pthread_mutex_unlock(&debug_mtx);
        sleep(2);
        pthread_mutex_lock(&debug_mtx);
        debug("Ride is over");
        debug("Train is entering monitor to empty");
        pthread_mutex_unlock(&debug_mtx);
        pthread_mutex_lock(&mtx);
        trainStop();
        pthread_mutex_unlock(&mtx);
        pthread_mutex_lock(&debug_mtx);
        if (trainStarted == 0){
            debug("Train is empty and exits monitor");
        }
        else{
            debug("Train exits monitor and passengers are debarkating");
        }
        pthread_mutex_unlock(&debug_mtx);
    }

    return NULL;
}

void *passenger(void *arg){
    int id;
    id = *(int *)arg;

    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d is entering monitor to embarkate", id);
    pthread_mutex_unlock(&debug_mtx);
    pthread_mutex_lock(&mtx);
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d is going to embarkate", id);
    pthread_mutex_unlock(&debug_mtx);
    embarkation(id);
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d embarkated and exits the monitor", id);
    pthread_mutex_unlock(&debug_mtx);
    pthread_mutex_unlock(&mtx);
    //Passenger wait for train to finish its ride to debarkate
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d is entering monitor to debarkate", id);
    pthread_mutex_unlock(&debug_mtx);
    pthread_mutex_lock(&mtx);
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d is going to debarkate", id);
    pthread_mutex_unlock(&debug_mtx);
    debarkation(id);
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d debarkated and exits the monitor", id);
    pthread_mutex_unlock(&debug_mtx);
    pthread_mutex_unlock(&mtx);
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d is done", id);
    pthread_mutex_unlock(&debug_mtx);

    return NULL;
}

void trainStart(){
    //debug("trainStarted = %d || trainFinished = %d", trainStarted, trainFinished);
    if(trainStarted == 0 || trainFinished == 1){
    	pthread_mutex_lock(&debug_mtx);
        debug("Train waits to full");
        pthread_mutex_unlock(&debug_mtx);
        pthread_cond_wait(&startTrain, &mtx);
    }
    else {
    	pthread_mutex_lock(&debug_mtx);
        debug("Train is full. Ride is going to start");
        pthread_mutex_unlock(&debug_mtx);
    }
}

void trainStop(){
    trainFinished = 1;
    pthread_mutex_lock(&debug_mtx);
    debug("Debarkation is about to begin");
    pthread_mutex_unlock(&debug_mtx);
    pthread_cond_signal(&debarkate);
}

void embarkation(int id){
    if(trainStarted == 1){
        /*Train is started.
        Other passengers must wait until train finish its ride. */
        blockedEmbarkation++;
        pthread_mutex_lock(&debug_mtx);
        debug("Passenger %d waits for train to finish its ride", id);
        pthread_mutex_unlock(&debug_mtx);
        pthread_cond_wait(&embarkate, &mtx);
    }
    nOfPassenger++;
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d is on train. Total number of passengers: %d",id, nOfPassenger);
    pthread_mutex_unlock(&debug_mtx);
    if(nOfPassenger == N){
        /*Train is full.
        Notify train to start the ride */
        pthread_mutex_lock(&debug_mtx);
        debug("Last passenger entered the train.Start the train");
        pthread_mutex_unlock(&debug_mtx);
        trainStarted = 1;
        pthread_cond_signal(&startTrain);
    }
    else{
        if(blockedEmbarkation != 0){
            blockedEmbarkation--;
            pthread_mutex_lock(&debug_mtx);
            debug("Passenger %d wakes up other blocked passenger", id);
            pthread_mutex_unlock(&debug_mtx);
            pthread_cond_signal(&embarkate);
        }
    }
}

void debarkation(int id){
    //debug("Passenger %d waits for train to finish its ride %d", id);
    if(trainFinished == 0){
        blockedDebarkation++;
        pthread_mutex_lock(&debug_mtx);
        debug("Passenger %d waits for train to finish its ride, to debarkate", id);
        pthread_mutex_unlock(&debug_mtx);
        pthread_cond_wait(&debarkate, &mtx);
    }

    nOfPassenger--;
    pthread_mutex_lock(&debug_mtx);
    debug("Passenger %d debarkated. Remaining passengers: %d", id, nOfPassenger);
    pthread_mutex_unlock(&debug_mtx);
    if (nOfPassenger == 0){
        /*Last passenger out of the train.
        Notify other passenger to embarkate */
        pthread_mutex_lock(&debug_mtx);
        debug("Last passenger (id = %d) debarkated", id);
        pthread_mutex_unlock(&debug_mtx);
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
