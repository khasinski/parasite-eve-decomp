/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_state.h"

extern char D_800118BC[];

void checkRECT(char *message, RECT *rect);

int ClearImage(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118BC, rect);
    color = ((u32)b << 16) | ((u32)g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}
