#include <stdlib.h>
#include <stdio.h>
#include "sentinel.h"


void initQueue(Queue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}


void enqueue(Queue* q, SentinelTCB* tcb) {
    if (tcb == NULL) return;
    
    tcb->next = NULL;
    if (q->tail == NULL) {
        // La cola estaba vacía
        q->head = tcb;
        q->tail = tcb;
    } else {
    
        q->tail->next = tcb;
        q->tail = tcb;
    }
    q->count++;
}


void enqueueAtHead(Queue* q, SentinelTCB* tcb) {
    if (tcb == NULL) return;
    
    tcb->next = q->head;
    q->head = tcb;
    
    if (q->tail == NULL) {

        q->tail = tcb;
    }
    q->count++;
}


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

bool isQueueEmpty(Queue* q) {
    return (q->count == 0);
}