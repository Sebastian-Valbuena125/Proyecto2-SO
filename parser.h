#ifndef PARSER_H
#define PARSER_H

#include "sentinel.h"

// Lee el archivo de escenario, crea los bloques TCB e inserta ordenadamente en la cola de arribos
int parseInputFile(const char* filename, Queue* arrivalQueue);

#endif