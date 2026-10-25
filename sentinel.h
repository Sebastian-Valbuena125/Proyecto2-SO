#ifndef SENTINEL_H
#define SENTINEL_H

#include <stdbool.h>

// Ciclo de vida y estados operativos de un centinela en el sistema
typedef enum {
    NEW,
    READY,
    RUNNING,
    INTERRUPTED,
    FINISHED
} ProcessState;

// Bloque de Control de Tarea (TCB): almacena el contexto de ejecución de cada centinela
typedef struct SentinelTCB {
    int id;                 // Identificador único
    ProcessState state;     // Estado de ejecución actual
    int priority;           // Nivel de prioridad (0: Comando, 1: Perforador, 2: Pesado)
    int arrivalTime;        // Tic global de arribo al sistema
    int totalBurst;         // Ráfaga total de procesamiento requerida
    int remainingBurst;     // Ráfaga restante por ejecutar
    int usedQuantum;        // Quantum consumido de forma continua en la ráfaga actual
    
    // Métricas de auditoría y rendimiento
    int waitTime;           // Tiempo total de espera en colas
    int returnTime;         // Tiempo total de retorno (turnaround)
    
    // Puntero para el encadenamiento en estructuras de datos
    struct SentinelTCB* next;
} SentinelTCB;

// Estructura lineal para la gestión de colas de procesos
typedef struct {
    SentinelTCB* head;      // Puntero al primer elemento (frente)
    SentinelTCB* tail;      // Puntero al último elemento (cola)
    int count;              // Cantidad actual de elementos encolados
} Queue;

// Primitivas de administración y manipulación de colas
void initQueue(Queue* q);
void enqueue(Queue* q, SentinelTCB* tcb);          
void enqueueAtHead(Queue* q, SentinelTCB* tcb);    
SentinelTCB* dequeue(Queue* q);                    
bool isQueueEmpty(Queue* q);
void enqueueSortedArrival(Queue* q, SentinelTCB* tcb);

#endif