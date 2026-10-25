#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <pthread.h>
#include <stdbool.h>
#include "sentinel.h"

// Colas jerárquicas del planificador multinivel
extern Queue q0; // Cola 0: Centinelas de Comando (Round Robin, Q=2)
extern Queue q1; // Cola 1: Centinelas Perforadores (Round Robin, Q=4)
extern Queue q2; // Cola 2: Centinelas de Desgaste (FCFS)

// Variables globales de control y estado de la simulación
extern int globalTick;
extern bool simulationActive;

// Primitivas POSIX para la barrera de sincronización entre hilos
extern pthread_mutex_t syncMutex;
extern pthread_cond_t masterCond;
extern pthread_cond_t workerCond;
extern bool isMasterTurn;

// Funciones de ciclo de vida del despachador
void initDispatcher();
void destroyDispatcher();

// Rutinas principales para los hilos de ejecución
void* masterThreadFunc(void* arg); // Hilo Maestro (Reloj y Admisión)
void* workerThreadFunc(void* arg); // Hilo Trabajador (Procesamiento en CPU_0)

#endif