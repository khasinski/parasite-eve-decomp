#ifndef PE1_GPU_QUEUE_H
#define PE1_GPU_QUEUE_H

#include "pe1/gpu_callbacks.h"

typedef struct GpuQueueState {
    unsigned char queue;
    unsigned char debugLevel;
} GpuQueueState;

extern GpuQueueState D_8009574D;
extern void (*D_80095748[])(char *message, int argument);
extern GpuCallbacks *D_80095744[];
extern char D_80011840[];

void DMACallback(int channel, void *callback);

#endif /* PE1_GPU_QUEUE_H */
