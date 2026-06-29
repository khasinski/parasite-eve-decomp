/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned short u_short;
typedef unsigned int u32;

typedef struct RECT {
    short x;
    short y;
    short w;
    short h;
} RECT;

int LoadImage(RECT *rect, u32 *p);
u_short GetTPage(int tp, int abr, int x, int y);

asm(".globl Gpu_LoadTexImageAndGetTPage");
asm("Gpu_LoadTexImageAndGetTPage = func_80074774");

u_short Gpu_LoadTexImageAndGetTPage(u32 *p, int tp, int abr, int x, int y, int w, int h)
    __asm__("func_80074774");

u_short Gpu_LoadTexImageAndGetTPage(u32 *p, int tp, int abr, int x, int y, int w, int h)
{
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
