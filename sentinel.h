#ifndef SENTINEL_H
#define SENTINEL_H

#include <stdbool.h>


typedef enum {
    NEW,
    READY,
    RUNNING,
    INTERRUPTED,
    FINISHED
} ProcessState;


typedef struct SentinelTCB {
    int id;                 
    ProcessState state;     
    int priority;           
    int arrivalTime;        
    int totalBurst;         
    int remainingBurst;     
    int usedQuantum;        
    
    
    int waitTime;           
    int returnTime;        
    

    struct SentinelTCB* next;
} SentinelTCB;


typedef struct {
    SentinelTCB* head;      
    SentinelTCB* tail;      
    int count;              
} Queue;


void initQueue(Queue* q);
void enqueue(Queue* q, SentinelTCB* tcb);          
void enqueueAtHead(Queue* q, SentinelTCB* tcb);    
SentinelTCB* dequeue(Queue* q);                    
bool isQueueEmpty(Queue* q);

#endif 