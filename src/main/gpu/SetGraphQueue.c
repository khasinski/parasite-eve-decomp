/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned char u8;

typedef struct GpuQueueState {
    u8 queue;
    u8 debug_level;
} GpuQueueState;

#include "pe1/gpu_callbacks.h"

extern GpuQueueState D_8009574D;
extern void (*D_80095748[])(char *msg, int arg0);
extern GpuCallbacks *D_80095744[];
extern char D_80011840[];

void DMACallback(int channel, void *callback);

int SetGraphQueue(int queue) {
    GpuQueueState *slot;
    int old;

    slot = &D_8009574D;
    old = slot->queue;

    if (slot->debug_level >= 2) {
        D_80095748[0](D_80011840, queue);
    }

    if (queue != slot->queue) {
        D_80095744[0]->reset(1);
        slot->queue = queue;
        DMACallback(2, 0);
    }

    return old;
}
