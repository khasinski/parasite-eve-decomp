/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned char u8;

#include "pe1/gpu_callbacks.h"

extern u8 D_8009574D[];
extern void (*D_80095748[])(char *msg, int arg0);
extern GpuCallbacks *D_80095744[];
extern char D_80011840[];

void DMACallback(int channel, void *callback);

int SetGraphQueue(int queue) {
    u8 *slot;
    int old;

    slot = D_8009574D;
    old = slot[0];

    if (slot[1] >= 2) {
        D_80095748[0](D_80011840, queue);
    }

    if (queue != slot[0]) {
        D_80095744[0]->reset(1);
        slot[0] = queue;
        DMACallback(2, 0);
    }

    return old;
}
