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

extern char D_800118E0[];
extern GpuCallbacks *g_GpuCallbacks;

void checkRECT(char *msg, RECT *rect);

int StoreImage(RECT *rect, u32 *p) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118E0, rect);
    callbacks = g_GpuCallbacks;
    return callbacks->addque2(callbacks->u1c.drs, rect, 8, p);
}
