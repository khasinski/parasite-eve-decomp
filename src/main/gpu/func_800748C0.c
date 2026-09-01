#include "common.h"
#include "pe1/psyq_gpu.h"

/* GCC_VERSION: 2.8.1 */

int LoadImage(RECT *rect, void *pixels);
u16 GetClut(int x, int y);

u32 func_800748C0(void *pixels, int x, int y)
{
    RECT rect;
    u32 clut;

    rect.x = x;
    rect.y = y;
    rect.w = 0x10;
    rect.h = 1;
    LoadImage(&rect, pixels);
    clut = GetClut(x, y);
    clut &= 0xFFFF;
    /* Keep the result normalization ahead of the callee-save restores. */
    __asm__ volatile("" : "+r"(clut));
    return clut;
}
