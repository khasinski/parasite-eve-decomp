/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_state.h"

extern char D_800118B8[], D_800118A4[];

void checkRECT(char *name, RECT *rect) {
    char *format;
    switch (D_8009574C.queueState.debugLevel) {
    case 1:
        if (rect->w > D_8009574C.width || rect->w + rect->x > D_8009574C.width ||
            rect->y > D_8009574C.height || rect->y + rect->h > D_8009574C.height ||
            rect->w <= 0 || rect->x < 0 || rect->y < 0 || rect->h <= 0) {
            /* USA D_80011898: keep page and string offset separate. */
            format = (char *)0x80010000;
            asm volatile("" : "=r"(format) : "0"(format));
            format += 0x1898;
            break;
        }
        return;
    case 2:
        format = D_800118B8;
        break;
    default:
        return;
    }
    D_80095748(format, name);
    {
        int x = rect->x;
        int y = rect->y;
        int width = rect->w;
        register int height asm("$3") = rect->h;
        D_80095748(D_800118A4, x, y, width, height);
    }
}

/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_state.h"

extern char D_800118BC[];
extern char D_800118C8[];

void checkRECT(char *message, RECT *rect);

int ClearImage(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118BC, rect);
    color = ((u32)b << 16) | ((u32)g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}

int ClearImage2(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118C8, rect);
    color = 0x80000000 | ((u32)b << 16) | ((u32)g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}
