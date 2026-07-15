/* MASPSX_FLAGS: --stack-return-delay --store-return-delay */

#include "pe1/psyq_gpu.h"

int LoadImage(RECT *rect, u_long *p);
u_short GetTPage(int tp, int abr, int x, int y);

u_short Gpu_LoadTexImageAndGetTPage(u_long *p, int tp, int abr, int x, int y, int w, int h) {
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.h = h;

    if (tp == 1) {
        goto mode1;
    }
    if (tp < 2) {
        if (tp == 0) {
            goto mode0;
        }
    } else {
        if (tp == 2) {
            goto mode2;
        }
    }
    goto load;

mode0:
    rect.w = w / 4;
    goto load_after_width;

mode1:
    rect.w = w / 2;
    goto load_after_width;

mode2:
    rect.w = w;

load_after_width:
load:
    LoadImage(&rect, p);
    return GetTPage(tp, abr, x, y) & 0xFFFF;
}
