/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"

extern char D_800118D4[];
extern GpuCallbacks *D_80095744;

void checkRECT(char *message, RECT *rect);

int LoadImage(RECT *rect, void *pixels) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118D4, rect);
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->u20.load, rect, 8, pixels);
}
