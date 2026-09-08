#include "common.h"
#include "pe1/psyq_gpu.h"

/* GCC_VERSION: 2.8.1 */

/* Psy-Q libgpu/sys.c tail: seven contiguous routines in SDK source order. */

int LoadImage(RECT *rect, u_long *pixels);
u_short GetTPage(int mode, int blend, int x, int y);
u_short GetClut(int x, int y);
int GetVideoMode(void);

extern int D_800956EC;

typedef struct VideoModeDataPage {
    char reserved00[0x56EC];
    int video_mode;
} VideoModeDataPage;

register VideoModeDataPage *g_VideoModeWritePage asm("$1");

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

u32 func_8007485C(void *pixels, int x, int y)
{
    RECT rect;
    u32 clut;

    rect.x = x;
    rect.y = y;
    rect.w = 0x100;
    rect.h = 1;
    LoadImage(&rect, pixels);
    clut = GetClut(x, y);
    clut &= 0xFFFF;
    /* Keep the result normalization ahead of the callee-save restores. */
    __asm__ volatile("" : "+r"(clut));
    return clut;
}

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

DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h)
{
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


int SetVideoMode(int mode) {
    int old;

    old = D_800956EC;
    g_VideoModeWritePage = (VideoModeDataPage *)0x80090000;
    g_VideoModeWritePage->video_mode = mode;
    return old;
}

int GetVideoMode(void) {
    return D_800956EC;
}
