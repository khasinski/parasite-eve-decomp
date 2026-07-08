/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_gpu.h"

typedef unsigned int u32;

#include "pe1/gpu_callbacks.h"

extern char D_800118D4[];
extern GpuCallbacks *g_GpuCallbacks;

void checkRECT(char *msg, RECT *rect);

int LoadImage(RECT *rect, u32 *p) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118D4, rect);
    callbacks = g_GpuCallbacks;
    return callbacks->addque2(callbacks->u20.dws, rect, 8, p);
}
