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

// Función auxiliar para imprimir el estado de las colas en modo Verbose
static void printQueueVerbose(Queue* q, const char* qName) {
    printf("  [%s]: ", qName);
    if (isQueueEmpty(q)) {
        printf("Empty\n");
    } else {
        SentinelTCB* curr = q->head;
        while (curr != NULL) {
            printf("[ID:%d] ", curr->id);
            curr = curr->next;
        }
        printf("\n");
    }
}

void* workerThreadFunc(void* arg) {
    (void)arg;
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

        // --- INICIO BLOQUE VERBOSE: ESTADO DEL SISTEMA ---
        if (verboseMode && (activeSentinel != NULL || !isQueueEmpty(&q0) || !isQueueEmpty(&q1) || !isQueueEmpty(&q2))) {
            printf("\n--- [VERBOSE] TICK %04d STATE ---\n", globalTick);
            printQueueVerbose(&q0, "Queue 0 (Q=2) ");
            printQueueVerbose(&q1, "Queue 1 (Q=4) ");
            printQueueVerbose(&q2, "Queue 2 (FCFS)");
            if (activeSentinel != NULL) {
                printf("  [CPU]: Sentinel ID: %d | Remaining Burst: %d | Quantum Used: %d\n", 
                       activeSentinel->id, activeSentinel->remainingBurst, activeSentinel->usedQuantum);
            } else {
                printf("  [CPU]: Idle\n");
            }
            printf("---------------------------------\n");
        }
        // --- FIN BLOQUE VERBOSE ---

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
                    if (verboseMode) {
                        printf("[T-%04d] [VERBOSE] QUANTUM_RENEWED: Sentinel %d renewed quantum (No competitors in Q0)\n",
                               globalTick, activeSentinel->id);
                    }
                    activeSentinel->usedQuantum = 0;
                }
            } else if (activeSentinel->priority == 1 && activeSentinel->usedQuantum == 4) {
                if (!isQueueEmpty(&q1)) {
                    printf("[T-%04d] QUANTUM_EXPIRED: Sentinel %d yields CPU via RR rotation (Q=4)\n", 
                           globalTick, activeSentinel->id);
                    expired = true;
                } else {
                    if (verboseMode) {
                        printf("[T-%04d] [VERBOSE] QUANTUM_RENEWED: Sentinel %d renewed quantum (No competitors in Q1)\n",
                               globalTick, activeSentinel->id);
                    }
                    activeSentinel->usedQuantum = 0;
                }
            }

            if (expired) {
                activeSentinel->usedQuantum = 0; 
                activeSentinel->state = READY;
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
                if (verboseMode) {
                    printf("[T-%04d] [VERBOSE] PREEMPTION: Sentinel %d preempted by higher priority\n", 
                           globalTick, activeSentinel->id);
                } else {
                    printf("[T-%04d] PREEMPTION: Sentinel %d preempted by higher priority\n", 
                           globalTick, activeSentinel->id);
                }
                
                activeSentinel->state = READY;
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
                if (verboseMode) {
                    printf("[T-%04d] [VERBOSE] DISPATCH: Sentinel %d selected for execution\n", globalTick, activeSentinel->id);
                }
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