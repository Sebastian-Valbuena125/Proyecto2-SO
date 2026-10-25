#include <stdlib.h>
#include <stdio.h>
#include "sentinel.h"

// Inicializa los punteros y el contador de una cola vacía
void initQueue(Queue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

// Inserta un centinela al final de la cola (comportamiento FIFO estándar)
void enqueue(Queue* q, SentinelTCB* tcb) {
    if (tcb == NULL) return;
    
    tcb->next = NULL;
    if (q->tail == NULL) {
        q->head = tcb;
        q->tail = tcb;
    } else {
        q->tail->next = tcb;
        q->tail = tcb;
    }
    q->count++;
}

// Inserta un centinela al frente de la cola (usado al ser expropiado)
void enqueueAtHead(Queue* q, SentinelTCB* tcb) {
    if (tcb == NULL) return;
    
    tcb->next = q->head;
    q->head = tcb;
    
    if (q->tail == NULL) {
        q->tail = tcb;
    }
    q->count++;
}

// Extrae y retorna el centinela ubicado a la cabeza de la cola
SentinelTCB* dequeue(Queue* q) {
    if (isQueueEmpty(q)) {
        return NULL;
    }
    
    SentinelTCB* temp = q->head;
    q->head = q->head->next;
    
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    q->count--;
    temp->next = NULL; 
    return temp;
}

// Verifica si la cola no contiene elementos
bool isQueueEmpty(Queue* q) {
    return (q->count == 0);
}

// Inserta centinelas ordenados ascendentemente por tiempo de llegada
void enqueueSortedArrival(Queue* q, SentinelTCB* tcb) {
    if (tcb == NULL) return;
    tcb->next = NULL;

    // Inserción al inicio si la cola está vacía o llega antes que la cabeza actual
    if (q->head == NULL || tcb->arrivalTime < q->head->arrivalTime) {
        tcb->next = q->head;
        q->head = tcb;
        if (q->tail == NULL) {
            q->tail = tcb;
        }
        q->count++;
        return;
    }

    // Búsqueda de la posición correspondiente manteniendo el orden de llegada (FIFO en empates)
    SentinelTCB* curr = q->head;
    while (curr->next != NULL && curr->next->arrivalTime <= tcb->arrivalTime) {
        curr = curr->next;
    }

    tcb->next = curr->next;
    curr->next = tcb;
    if (tcb->next == NULL) {
        q->tail = tcb;
    }
    q->count++;
}