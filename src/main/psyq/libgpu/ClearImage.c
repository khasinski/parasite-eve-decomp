#include "common.h"

#include "pe1/psyq_gpu.h"

#include "pe1/gpu_callbacks.h"

extern char D_800118BC[];
extern GpuCallbacks *g_GpuCallbacks;

void checkRECT(char *msg, RECT *rect);

int ClearImage(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118BC, rect);
    callbacks = g_GpuCallbacks;
    return callbacks->addque2(callbacks->clr, rect, 8, (b << 16) | (g << 8) | r);
}
