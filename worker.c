#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dispatcher.h"
#include "sentinel.h"

extern bool verboseMode;
extern Queue arrivalQueue;

int sumTurnaroundTime = 0;
int sumWaitTime = 0;
int totalProcessedSentinels = 0;

void* workerThreadFunc(void* arg) {
    SentinelTCB* activeSentinel = NULL;

    while (simulationActive) {
        pthread_mutex_lock(&syncMutex);

        while (isMasterTurn && simulationActive) {
            pthread_cond_wait(&workerCond, &syncMutex);
        }

        if (!simulationActive) {
            pthread_mutex_unlock(&syncMutex);
            break;
        }

        // 1. Fase de Finalización
        if (activeSentinel != NULL && activeSentinel->remainingBurst == 0) {
            activeSentinel->state = FINISHED;
            
            int turnaround = globalTick - activeSentinel->arrivalTime;
            int wait = turnaround - activeSentinel->totalBurst;
            
            sumTurnaroundTime += turnaround;
            sumWaitTime += wait;
            totalProcessedSentinels++;

            printf("[T-%04d] COMPLETED: Sentinel ID: %d -> Turnaround Time: %d ticks\n", 
                   globalTick, activeSentinel->id, turnaround);
            
            free(activeSentinel);
            activeSentinel = NULL;
        }

        // 2. Fase de Expiración de Quantum y Rotación
        if (activeSentinel != NULL) {
            bool expired = false;
            if (activeSentinel->priority == 0 && activeSentinel->usedQuantum == 2) {
                if (!isQueueEmpty(&q0)) {
                    printf("[T-%04d] QUANTUM_EXPIRED: Sentinel %d yields CPU via RR rotation (Q=2)\n", 
                           globalTick, activeSentinel->id);
                    expired = true;
                } else {
                    activeSentinel->usedQuantum = 0; // Se renueva el quantum sin rotar
                }
            } else if (activeSentinel->priority == 1 && activeSentinel->usedQuantum == 4) {
                if (!isQueueEmpty(&q1)) {
                    printf("[T-%04d] QUANTUM_EXPIRED: Sentinel %d yields CPU via RR rotation (Q=4)\n", 
                           globalTick, activeSentinel->id);
                    expired = true;
                } else {
                    activeSentinel->usedQuantum = 0; // Se renueva el quantum sin rotar
                }
            }

            if (expired) {
                activeSentinel->usedQuantum = 0; 
                activeSentinel->state = READY; // CORRECCIÓN: Actualizar el estado del TCB al abandonar la CPU
                Queue* targetQ = (activeSentinel->priority == 0) ? &q0 : &q1;
                enqueue(targetQ, activeSentinel); 
                activeSentinel = NULL;
            }
        }

        // 3. Fase de Expropiación (Preemption)
        if (activeSentinel != NULL) {
            bool preempt = false;
            if (activeSentinel->priority == 1 && !isQueueEmpty(&q0)) {
                preempt = true; 
            } else if (activeSentinel->priority == 2 && (!isQueueEmpty(&q0) || !isQueueEmpty(&q1))) {
                preempt = true; 
            }

            if (preempt) {
                // CORRECCIÓN: Envolver la impresión interna en el condicional del modo depuración
                if (verboseMode) {
                    printf("[T-%04d] [VERBOSE] PREEMPTION: Sentinel %d preempted by higher priority\n", 
                           globalTick, activeSentinel->id);
                }
                
                activeSentinel->state = READY; // CORRECCIÓN: Actualizar el estado del TCB al ser expropiado
                Queue* targetQ = (activeSentinel->priority == 1) ? &q1 : &q2;
                
                enqueueAtHead(targetQ, activeSentinel);
                activeSentinel = NULL;
            }
        }

        // 4. Fase de Despacho (Dispatch)
        if (activeSentinel == NULL) {
            if (!isQueueEmpty(&q0)) {
                activeSentinel = dequeue(&q0);
            } else if (!isQueueEmpty(&q1)) {
                activeSentinel = dequeue(&q1);
            } else if (!isQueueEmpty(&q2)) {
                activeSentinel = dequeue(&q2);
            }

            if (activeSentinel != NULL && activeSentinel->state != RUNNING) {
                activeSentinel->state = RUNNING;
            }
        }

        // 5. Ejecución de Ráfaga
        if (activeSentinel != NULL) {
            activeSentinel->remainingBurst--;

            if (activeSentinel->priority == 0 || activeSentinel->priority == 1) {
                activeSentinel->usedQuantum++;
            }
        }

        // 6. Verificación de Fin de Simulación
        if (isQueueEmpty(&arrivalQueue) && isQueueEmpty(&q0) && 
        isQueueEmpty(&q1) && isQueueEmpty(&q2) && activeSentinel == NULL) {
            simulationActive = false;
            isMasterTurn = true;
            pthread_cond_signal(&masterCond);
            pthread_mutex_unlock(&syncMutex);
            break;
        }

        // Ceder el turno de vuelta al Hilo Maestro
        isMasterTurn = true;
        pthread_cond_signal(&masterCond);
        pthread_mutex_unlock(&syncMutex);
    }

    return NULL;
}