/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_gpu.h"

typedef unsigned int u32;

#include "pe1/gpu_callbacks.h"

extern char D_800118D4[];
extern GpuCallbacks *g_GpuCallbacks;

void checkRECT(char *msg, RECT *rect);

int LoadImage(RECT *rect, u32 *p) {
    checkRECT(D_800118D4, rect);
    return g_GpuCallbacks->addque2(g_GpuCallbacks->u20.dws, rect, 8, p);
}
