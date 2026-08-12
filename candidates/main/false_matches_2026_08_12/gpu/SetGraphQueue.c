#include "common.h"

#include "pe1/gpu_queue.h"

int SetGraphQueue(int queue) {
    GpuQueueState *state = &D_8009574D;
    int previousQueue = state->queue;

    if (((volatile GpuQueueState *)state)->debugLevel >= 2) {
        D_80095748[0](D_80011840, queue);
    }

    if (queue != state->queue) {
        D_80095744[0]->reset(1);
        state->queue = queue;
        DMACallback(2, 0);
    }

    return previousQueue;
}
