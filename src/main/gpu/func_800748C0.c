#include "common.h"
#include "pe1/psyq_gpu.h"

/* MASPSX_FLAGS: --stack-return-delay */

int LoadImage(RECT *rect, void *pixels);
u16 GetClut(int x, int y);

u16 func_800748C0(void *pixels, int x, int y)
{
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 0x10;
    rect.h = 1;
    LoadImage(&rect, pixels);
    return GetClut(x, y);
}
