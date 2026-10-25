#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Lee el archivo de escenario y carga los TCB en la cola de arribos
int parseInputFile(const char* filename, Queue* arrivalQueue) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo el archivo de entrada");
        return -1;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // Ignora comentarios y líneas en blanco
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        int arrivalTime, id, priority, totalBurst;
        
        // Extrae los atributos del centinela e inicializa su bloque TCB
        if (sscanf(line, "%d %d %d %d", &arrivalTime, &id, &priority, &totalBurst) == 4) {
            SentinelTCB* tcb = (SentinelTCB*)malloc(sizeof(SentinelTCB));
            if (!tcb) continue;

            tcb->id = id;
            tcb->state = NEW;
            tcb->priority = priority;
            tcb->arrivalTime = arrivalTime;
            tcb->totalBurst = totalBurst;
            tcb->remainingBurst = totalBurst;
            tcb->usedQuantum = 0;
            tcb->waitTime = 0;
            tcb->returnTime = 0;
            tcb->next = NULL;

            // Encola ordenadamente según el tic de llegada
            enqueueSortedArrival(arrivalQueue, tcb);
        }
    }
    
    fclose(file);
    return 0;
}