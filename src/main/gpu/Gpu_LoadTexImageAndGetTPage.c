#include "pe1/psyq_gpu.h"

/* MASPSX_FLAGS: --stack-return-delay */

int LoadImage(RECT *rect, u_long *pixels);
u_short GetTPage(int mode, int blend, int x, int y);

u_short Gpu_LoadTexImageAndGetTPage(u_long *pixels, int mode, int blend,
                                    int x, int y, int width, int height)
{
    RECT rect;

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
    return GetTPage(mode, blend, x, y) & 0xFFFF;
}
