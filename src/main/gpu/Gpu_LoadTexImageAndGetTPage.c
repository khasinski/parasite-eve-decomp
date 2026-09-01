#include "common.h"
#include "pe1/psyq_gpu.h"

/* GCC_VERSION: 2.8.1 */

int LoadImage(RECT *rect, u_long *pixels);
u_short GetTPage(int mode, int blend, int x, int y);

u32 Gpu_LoadTexImageAndGetTPage(u_long *pixels, int mode, int blend,
                                    int x, int y, int width, int height)
{
    RECT rect;
    u32 tpage;
    register int savedBlend __asm__("$18") = blend;

    /* Preserve the original blend-before-x prologue schedule. */
    __asm__ volatile("" : "+r"(savedBlend));
    rect.x = x;
    rect.y = y;
    rect.h = height;

    if (mode == 1) {
        goto mode1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto mode0;
        }
    } else if (mode == 2) {
        goto mode2;
    }
    goto load;

mode0:
    rect.w = width / 4;
    goto loadWidth;
mode1:
    rect.w = width / 2;
    goto loadWidth;
mode2:
    rect.w = width;
loadWidth:
load:
    LoadImage(&rect, pixels);
    tpage = GetTPage(mode, savedBlend, x, y);
    tpage &= 0xFFFF;
    /* Keep the result normalization ahead of the callee-save restores. */
    __asm__ volatile("" : "+r"(tpage));
    return tpage;
}
