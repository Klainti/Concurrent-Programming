#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "debug.h"

pthread_mutex_t mtx,main_done;
pthread_cond_t queue[2];
int nOfcars, carsBrigde;
volatile int crossing[2]={0,0}; // index 0:red , index 1:blue
volatile int blocked[2]={0,0}; // index 0:red , index 1:blue
volatile int car_crossed=0;

void *car(void *arg);
void cross(int side);
void quit(int side);

int main(int argc,char *argv[]){

    int i,iret,*color_car,time_car;
    pthread_t car_thread;

    if (argc<3){
        debug_e("Give size of the bridge and number of cars!");
        return(EXIT_FAILURE);
    }

    debug("Init semaphores");
    pthread_mutex_init(&main_done,NULL);
    pthread_mutex_lock(&main_done);
    pthread_mutex_init(&mtx,NULL);
    pthread_cond_init(&queue[0],NULL);
    pthread_cond_init(&queue[1],NULL);
    debug("Done with initialize");
    
    carsBrigde = atoi(argv[1]);
    nOfcars = atoi(argv[2]);

    color_car = (int *)calloc(nOfcars,sizeof(int));
    if (color_car==NULL){
        debug_e("Callc failed!");
        return(EXIT_FAILURE);
    }

    for (i=0; i<nOfcars; i++){
        scanf("%d %d",color_car+i,&time_car);
        sleep(time_car);
        iret = pthread_create(&car_thread,NULL,&car,(void *) (color_car+i));
        if (iret){
            debug_e("pthread failed");
            return(EXIT_FAILURE);
        }
    }

    pthread_mutex_lock(&main_done);
    debug("Exit Main");
   return(0);
}

void cross(int side){
    debug("Begin side: %s",side ? "Blue" : "Red");

    if (crossing[1-side] + blocked[1-side]>0){ //Check for other side
        //Give priority to other side
        debug("Wait for other side to cross the bridge");
        blocked[side]++;
        debug("Blocked: %d",blocked[side]);
        debug("Enter queue[%d]",side);
        pthread_cond_wait(&queue[side],&mtx);
        debug("Check for crossing the road--> cross:%d and blocked:%d",crossing[side],blocked[side]);
        if (blocked[side]>0 && crossing[side]<carsBrigde){//check for crossing the road
            debug("Car from side:%s is going to cross the road",side ? "Blue" : "Red");
            blocked[side]--;
            crossing[side]++;
            debug("Exit queue[%d]",side); 
            pthread_cond_signal(&queue[side]); //notify other car to cross the bridge
        }
        else{
            debug("Can't cross the road,side:%s",side ? "Blue" : "Red");
            pthread_cond_wait(&queue[side],&mtx);
            blocked[side]--;
            crossing[side]++;
        }
    }
    else{
        debug("No cars from opposite side");
        if (crossing[side]<carsBrigde){
            crossing[side]++;
       }
        else{
            debug("Bridge full , side:%s",side ? "Blue" : "Red");
            blocked[side]++;
            debug("Blocked: %d",blocked[side]);
            debug("Enter queue[%d]",side);
            pthread_cond_wait(&queue[side],&mtx);
            debug("Check for crossing the road--> cross:%d and blocked:%d",crossing[side],blocked[side]);
            if (blocked[side]>0 && crossing[side]<carsBrigde){//check for crossing the road
                debug("Car from side:%s is going to cross the road",side ? "Blue" : "Red");
                blocked[side]--;
                crossing[side]++;
                debug("Exit queue[%d]",side); 
                pthread_cond_signal(&queue[side]); //notify other car to cross the bridge
            }
            else{
                debug("Can't cross the road,side:%s,Unlock mtx",side ? "Blue" : "Red");
                pthread_cond_wait(&queue[side],&mtx);
                blocked[side]--;
                crossing[side]++;    
            }
        }
    }
}

void quit(int side){

    debug("Enter Exit code");
    crossing[side]--;
    debug("Cars still in the bridge: %d",crossing[side]);
    if (crossing[side]==0){
        if (blocked[1-side]>0){
            debug("Last car, notify others side");
            pthread_cond_signal(&queue[1-side]);
        }
        else if (blocked[side]>0){
            debug("Last car, notify same side");
            pthread_cond_signal(&queue[side]);
        }
   }

    car_crossed++;
    if (car_crossed==nOfcars){
        pthread_mutex_unlock(&main_done);
    }
}


void *car(void *arg){

    int side = *(int *)arg;

    pthread_mutex_lock(&mtx);
    cross(side);
    pthread_mutex_unlock(&mtx);

    sleep(1);

    pthread_mutex_lock(&mtx);
    quit(side);
    pthread_mutex_unlock(&mtx);

    return NULL;
}
