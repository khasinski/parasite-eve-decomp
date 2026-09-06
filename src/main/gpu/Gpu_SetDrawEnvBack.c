/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"
extern short D_80095750, D_80095752;
unsigned int Gpu_BuildDrawAreaTopLeftCmd(short, short);
unsigned int Gpu_BuildDrawAreaBottomRightCmd(short, short);
unsigned int Gpu_BuildDrawOffsetCmd(short, short);
unsigned int Gpu_BuildDrawModeCmd(int, int, int);
unsigned int Gpu_BuildTexWindowCmd(RECT *);

void Gpu_SetDrawEnvBack(DR_ENV *packet, DRAWENV *env)
{
    RECT rect;
    unsigned int *words = (unsigned int *)packet;
    int n;
    words[1] = Gpu_BuildDrawAreaTopLeftCmd(env->clip.x, env->clip.y);
    words[2] = Gpu_BuildDrawAreaBottomRightCmd(
        env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    words[3] = Gpu_BuildDrawOffsetCmd(env->ofs[0], env->ofs[1]);
    words[4] = Gpu_BuildDrawModeCmd(env->dfe, env->dtd, env->tpage);
    words[5] = Gpu_BuildTexWindowCmd(&env->tw);
    words[6] = 0xE6000000;
    n = 7;
    if (env->isbg) {
        rect.x = env->clip.x;
        rect.y = env->clip.y;
        rect.w = env->clip.w;
        rect.h = env->clip.h;
        rect.w = rect.w < 0 ? 0 : rect.w > D_80095750 - 1 ? D_80095750 - 1 : rect.w;
        rect.h = rect.h < 0 ? 0 : rect.h > D_80095752 - 1 ? D_80095752 - 1 : rect.h;
        if ((rect.x & 63) || (rect.w & 63)) {
            rect.x -= env->ofs[0];
            rect.y -= env->ofs[1];
            words[n++] = 0x60000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
            words[n++] = *(unsigned int *)&rect.x;
            words[n++] = *(unsigned int *)&rect.w;
        } else {
            words[n++] = 0x02000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
            words[n++] = *(unsigned int *)&rect.x;
            words[n++] = *(unsigned int *)&rect.w;
        }
    }
    ((unsigned char *)words)[3] = n - 1;
}
