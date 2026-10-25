#include "dispatcher.h"

// Colas multinivel de prioridad estricta
Queue q0, q1, q2;

// Variables de control del reloj global y estado de la simulación
int globalTick = 0;
bool simulationActive = true;
bool isMasterTurn = true; 

// Primitivas para la sincronización por barrera de fase entre hilos
pthread_mutex_t syncMutex;
pthread_cond_t masterCond;
pthread_cond_t workerCond;

// Inicializa las colas del planificador y los objetos de sincronización POSIX
void initDispatcher() {
    initQueue(&q0);
    initQueue(&q1);
    initQueue(&q2);
    pthread_mutex_init(&syncMutex, NULL);
    pthread_cond_init(&masterCond, NULL);
    pthread_cond_init(&workerCond, NULL);
}

// Destruye el mutex y las variables de condición al finalizar la ejecución
void destroyDispatcher() {
    pthread_mutex_destroy(&syncMutex);
    pthread_cond_destroy(&masterCond);
    pthread_cond_destroy(&workerCond);
}