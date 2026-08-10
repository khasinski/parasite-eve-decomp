/* MASPSX_FLAGS: --stack-return-delay --store-return-delay */

/* PSY-Q libgpu sys.c utility tail, compiled into the game as one
 * translation unit (LoadTPage..GetVideoMode in SDK order). */

#include "pe1/psyq_gpu.h"

extern int g_VideoMode;

int LoadImage(RECT *rect, u_long *p);
u_short GetTPage(int tp, int abr, int x, int y);
u_short GetClut(int x, int y);
int GetVideoMode(void);

u_short Gpu_LoadTexImageAndGetTPage(u_long *p, int tp, int abr, int x, int y, int w, int h)
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

u_short LoadClut(u_long *p, int x, int y) {
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 0x100;
    rect.h = 1;
    LoadImage(&rect, p);
    return GetClut(x, y);
}

u_short LoadClut2(u_long *p, int x, int y) {
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 0x10;
    rect.h = 1;
    LoadImage(&rect, p);
    return GetClut(x, y);
}

DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h) {
    int videoMode;

    videoMode = GetVideoMode();
    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    env->dtd = 1;
    env->clip.h = h;

    if (videoMode != 0) {
        env->dfe = h < 0x121;
    } else {
        env->dfe = h < 0x101;
    }

    env->ofs[0] = x;
    env->ofs[1] = y;
    env->tpage = 0xA;
    env->isbg = 0;
    return env;
}

DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h) {
    env->disp.x = x;
    env->disp.y = y;
    env->disp.w = w;
    env->disp.h = h;
    env->screen.x = 0;
    env->screen.y = 0;
    env->screen.w = 0;
    env->screen.h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}

int SetVideoMode(int arg0) {
    int old = g_VideoMode;
    g_VideoMode = arg0;
    return old;
}

int GetVideoMode(void) {
    return g_VideoMode;
}
