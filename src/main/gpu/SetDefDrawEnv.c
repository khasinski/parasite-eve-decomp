/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_gpu.h"

int GetVideoMode(void);

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
