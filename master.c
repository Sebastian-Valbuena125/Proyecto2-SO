#include <stdio.h>
#include <stdlib.h>
#include "dispatcher.h"
#include "sentinel.h"

extern bool verboseMode;

const char* getQueueName(int priority) {
    if (priority == 0) return "Command";
    if (priority == 1) return "Driller";
    return "Heavy";
}

void* masterThreadFunc(void* arg) {
    Queue* arrivalQueue = (Queue*)arg;

    while (simulationActive) {
        pthread_mutex_lock(&syncMutex);

    
        while (arrivalQueue->head != NULL && arrivalQueue->head->arrivalTime == globalTick) {
            SentinelTCB* incoming = dequeue(arrivalQueue);
            incoming->state = READY;

            printf("[T-%04d] ARRIVAL: Sentinel ID: %d [Type: %s]\n", 
                   globalTick, incoming->id, getQueueName(incoming->priority));

           
            if (incoming->priority == 0) {
                enqueue(&q0, incoming);
            } else if (incoming->priority == 1) {
                enqueue(&q1, incoming);
            } else if (incoming->priority == 2) {
                enqueue(&q2, incoming);
            }
        }

   
        isMasterTurn = false;
        pthread_cond_signal(&workerCond);

        while (!isMasterTurn && simulationActive) {
            pthread_cond_wait(&masterCond, &syncMutex);
        }

        if (simulationActive) {
            globalTick++;
        }

        pthread_mutex_unlock(&syncMutex);
    }

    return NULL;
}