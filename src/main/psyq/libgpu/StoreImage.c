/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_gpu.h"

typedef unsigned int u32;

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
