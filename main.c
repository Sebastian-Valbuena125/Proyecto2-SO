#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "sentinel.h"
#include "parser.h"
#include "dispatcher.h"
#include "metrics.h"

// Variables globales de configuración y cola inicial de arribos
bool verboseMode = false;
char* inputFileName = NULL;
Queue arrivalQueue;

// Procesa y valida los argumentos pasados por línea de comandos
void parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "-verbose") == 0) {
            verboseMode = true;
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            inputFileName = argv[i + 1];
            i++;
        } else {
            fprintf(stderr, "Uso: %s [-v] --input <input_file>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (inputFileName == NULL) {
        fprintf(stderr, "Error: Se requiere el parámetro --input <input_file>\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    // Lectura de parámetros y carga inicial del escenario de entrada
    parseArguments(argc, argv);
    initQueue(&arrivalQueue);

    if (parseInputFile(inputFileName, &arrivalQueue) != 0) {
        return EXIT_FAILURE;
    }

    // Impresión de cabecera con los parámetros fijos del sistema
    printf("Fixed parameters: Queue 0 [Q=2] | Queue 1 [Q=4] | Queue 2 [FCFS]\n");

    if (verboseMode) {
        printf("[VERBOSE] Archivo leido correctamente. Total centinelas: %d\n", arrivalQueue.count);
    }

    // Inicialización de colas de prioridad y primitivas de sincronización
    initDispatcher();

    pthread_t masterThread, workerThread;

    // Creación de los hilos Maestro (reloj/admisión) y Trabajador (CPU_0)
    pthread_create(&masterThread, NULL, masterThreadFunc, &arrivalQueue);
    pthread_create(&workerThread, NULL, workerThreadFunc, NULL);

    // Espera a que ambos hilos concluyan la simulación
    pthread_join(masterThread, NULL);
    pthread_join(workerThread, NULL);

    // Liberación de recursos de sincronización y reporte de métricas finales
    destroyDispatcher();
    printFinalMetrics();

    return EXIT_SUCCESS;
}
