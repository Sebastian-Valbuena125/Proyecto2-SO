#include <stdio.h>
#include "metrics.h"
#include "dispatcher.h"

extern int sumWaitTime;
extern int totalProcessedSentinels;

void printFinalMetrics() {
    float throughput = 0.0;
    float avgWaitTime = 0.0;

    if (globalTick > 0) {
        throughput = (float)totalProcessedSentinels / globalTick;
    }
    
    if (totalProcessedSentinels > 0) {
        avgWaitTime = (float)sumWaitTime / totalProcessedSentinels;
    }

    printf("====== GLOBAL PERFORMANCE METRICS ====================\n");
    printf("Total Simulation Time  : %d clock ticks.\n", globalTick);
    printf("System Throughput      : %.2f sentinels/tick.\n", throughput);
    printf("Average Waiting Time   : %.2f ticks.\n", avgWaitTime);
    printf("======================================================\n");
}