#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <pthread.h>
#include <stdbool.h>
#include "sentinel.h"

extern Queue q0; 
extern Queue q1; 
extern Queue q2; 
extern int globalTick;
extern bool simulationActive;
extern pthread_mutex_t syncMutex;
extern pthread_cond_t masterCond;
extern pthread_cond_t workerCond;
extern bool isMasterTurn;

void initDispatcher();
void destroyDispatcher();
void* masterThreadFunc(void* arg);
void* workerThreadFunc(void* arg);

#endif 