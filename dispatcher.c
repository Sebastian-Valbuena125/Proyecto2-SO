#include "dispatcher.h"

Queue q0, q1, q2;

int globalTick = 0;
bool simulationActive = true;
bool isMasterTurn = true; 

pthread_mutex_t syncMutex;
pthread_cond_t masterCond;
pthread_cond_t workerCond;

void initDispatcher() {
   
    initQueue(&q0);
    initQueue(&q1);
    initQueue(&q2);
    pthread_mutex_init(&syncMutex, NULL);
    pthread_cond_init(&masterCond, NULL);
    pthread_cond_init(&workerCond, NULL);

}


void destroyDispatcher() {

    pthread_mutex_destroy(&syncMutex);
    pthread_cond_destroy(&masterCond);
    pthread_cond_destroy(&workerCond);
    
}