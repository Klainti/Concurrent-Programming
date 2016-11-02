#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "debug.h"

pthread_mutex_t mtx,main_done;
pthread_mutex_t queue[2];
int nOfcars, carsBrigde;
volatile int crossing[2]={0,0}; // index 0:red , index 1:blue
volatile int blocked[2]={0,0}; // index 0:red , index 1:blue
volatile int car_crossed=0;

void *car(void *arg);

int main(int argc,char *argv[]){

    int i,iret,color_car;
    pthread_t car_thread;

    debug("Init semaphores");
    pthread_mutex_init(&main_done,NULL);
    pthread_mutex_lock(&main_done);
    pthread_mutex_init(&mtx,NULL);
    pthread_mutex_init(&queue[0],NULL);
    pthread_mutex_lock(&queue[0]);
    pthread_mutex_init(&queue[1],NULL);
    pthread_mutex_lock(&queue[1]);
    debug("Done with initialize");

    printf("Give size of the bridge:");
    scanf("%d",&carsBrigde);

    printf("Give number of cars:");
    scanf("%d",&nOfcars);

    srand(time(NULL));
    for (i=0; i<nOfcars; i++){
        color_car = rand() % 2;
        //debug_e("New car: %s", color_car ? "Red" : "Blue");
        iret = pthread_create(&car_thread,NULL,&car,(void *) &color_car);
        if (iret){
            debug_e("pthread failed");
            return(EXIT_FAILURE);
        }
    }

    pthread_mutex_lock(&main_done);
    debug("Exit Main");
    return(0);
}

void *car(void *arg){

    int side = *(int *)arg;
    debug("Begin side: %d",side);

    debug("Lock mtx");
    pthread_mutex_lock(&mtx);
    if (crossing[1-side] + blocked[1-side]>0){ //Check for other side
        //Give priority to other side
        debug("Wait for other side to cross the bridge");
        blocked[side]++;
        debug("Blocked: %d",blocked[side]);
        debug("Unlock mtx");
        pthread_mutex_unlock(&mtx);
        debug("Lock queue[%d]",side);
        pthread_mutex_lock(&queue[side]);
        debug("Check for crossing the road--> cross:%d and blocked:%d",crossing[side],blocked[side]);
        if (blocked[side]>0 && crossing[side]<carsBrigde){//check for crossing the road
            debug("Car from side:%d is going to cross the road",side);
            blocked[side]--;
            crossing[side]++;
            debug("Unlock queue[%d]",side); 
            pthread_mutex_unlock(&queue[side]); //notify other car to cross the bridge
        }
        else{
            debug("Can't cross the road,side:%d,Unlock mtx",side);
            pthread_mutex_unlock(&mtx);
            pthread_mutex_lock(&queue[side]);
            blocked[side]--;
            crossing[side]++;
        }
    }
    else{
        debug("No cars from opposite side %d",side);
        if (crossing[side]<carsBrigde){
            crossing[side]++;
            debug("Unlock mtx--> crossing:%d",crossing[side]);
            pthread_mutex_unlock(&mtx);
        }
        else{
            debug("Bridge full , side:%d",side);
            blocked[side]++;
            debug("Blocked: %d",blocked[side]);
            debug("Unlock mtx");
            pthread_mutex_unlock(&mtx);
            debug("Lock queue[%d]",side);
            pthread_mutex_lock(&queue[side]);
            debug("Check for crossing the road--> cross:%d and blocked:%d",crossing[side],blocked[side]);
            if (blocked[side]>0 && crossing[side]<carsBrigde){//check for crossing the road
                debug("Car from side:%d is going to cross the road",side);
                blocked[side]--;
                crossing[side]++;
                debug("Unlock queue[%d]",side); 
                pthread_mutex_unlock(&queue[side]); //notify other car to cross the bridge
            }
            else{
                debug("Can't cross the road,side:%d,Unlock mtx",side);
                pthread_mutex_unlock(&mtx);
                pthread_mutex_lock(&queue[side]);
                blocked[side]--;
                crossing[side]++;    
            }
        }
    }

    debug("Cross the bridge");
    sleep(1);


    pthread_mutex_lock(&mtx);
    debug("Enter CS/exit code");
    crossing[side]--;
    debug("Cars still in the bridge: %d",crossing[side]);
    if (crossing[side]==0){
        if (blocked[1-side]>0){
            debug("Last car, notify others side");
            pthread_mutex_unlock(&queue[1-side]);
        }
        else if (blocked[side]>0){
            debug("Last car, notify same side");
            pthread_mutex_unlock(&queue[side]);
        }
        /*
        else{
            debug("No cars on the road!");
            pthread_mutex_unlock(&mtx);
        }
        */
    }
    debug("Unlock mtx/exit code");
    car_crossed++;
    if (car_crossed==nOfcars){
        pthread_mutex_unlock(&mtx);
        pthread_mutex_unlock(&main_done);
    }
    else{
        pthread_mutex_unlock(&mtx);
    }

    return NULL;
}
