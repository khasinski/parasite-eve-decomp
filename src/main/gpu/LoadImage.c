/* MASPSX_FLAGS: --stack-return-delay */

typedef signed short s16;
typedef unsigned int u32;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

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
