#include "pe1/gpu_command_builders.h"
/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ part: GetDispEnv, GetODE, SetTexWindow, SetDrawArea,
 * SetDrawOffset, SetPriority, SetDrawStp, SetDrawMode, SetDrawEnv and
 * Gpu_SetDrawEnvBack.
 * The other SYS.OBJ functions are in neighbouring units because their
 * reconstructions need different compiler options.
 */
#include "pe1/gpu_state.h"
#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "pe1/draw_area.h"

extern void *memcpy(void *dest, const void *src, unsigned int n);
extern char g_GpuActiveDispEnv[];

void *GetDispEnv(void *arg0) {
    void *(*fn)(void *, const void *, unsigned int);

    fn = memcpy;
    fn(arg0, g_GpuActiveDispEnv, 0x14);
    return arg0;
}

int GetODE(void) {
    return D_80095744->callback() < 0;
}



typedef signed short s16_1;

typedef struct {
    s16_1 x;
    s16_1 y;
} Point;


void SetTexWindow(GpuCmdPacket *arg0, int arg1) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildTexWindowCmd((GpuTextureWindowRectBytes *)arg1);
    arg0->field8 = 0;
}

void SetDrawArea(GpuCmdPacket *arg0, DrawAreaRect *arg1) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildDrawAreaTopLeftCmd(arg1->x, arg1->y);
    arg0->field8 = Gpu_BuildDrawAreaBottomRightCmd((s16)(arg1->x + arg1->w - 1), (s16)(arg1->y + arg1->h - 1));
}

void SetDrawOffset(GpuCmdPacket *arg0, Point *arg1) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildDrawOffsetCmd(arg1->x, arg1->y);
    arg0->field8 = 0;
}

void SetPriority(void *arg0, int arg1, int arg2) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) =
        (arg1 ? 0xE6000002 : 0xE6000000) | (arg2 != 0);
    *(int *)((char *)arg0 + 8) = 0;
}

void SetDrawStp(void *arg0, int arg1) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) = arg1 ? 0xE6000001 : 0xE6000000;
    *(int *)((char *)arg0 + 8) = 0;
}


void SetDrawMode(GpuCmdPacket *arg0, int arg1, int arg2, int arg3, int arg4) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildDrawModeCmd(arg1, arg2, arg3 & 0xFFFF);
    arg0->field8 = Gpu_BuildTexWindowCmd((GpuTextureWindowRectBytes *)arg4);
}

void SetDrawEnv(DR_ENV *packet, DRAWENV *env)
{
    RECT rect;
    unsigned int *words = (unsigned int *)packet;
    int n;
    words[1] = Gpu_BuildDrawAreaTopLeftCmd(env->clip.x, env->clip.y);
    words[2] = Gpu_BuildDrawAreaBottomRightCmd(
        (short)(env->clip.w + env->clip.x - 1), (short)(env->clip.y + env->clip.h - 1));
    words[3] = Gpu_BuildDrawOffsetCmd(env->ofs[0], env->ofs[1]);
    words[4] = Gpu_BuildDrawModeCmd(env->dfe, env->dtd, env->tpage);
    words[5] = Gpu_BuildTexWindowCmd((GpuTextureWindowRectBytes *)&env->tw);
    words[6] = 0xE6000000;
    n = 7;
    if (env->isbg) {
        rect.x = env->clip.x;
        rect.y = env->clip.y;
        rect.w = env->clip.w;
        rect.h = env->clip.h;
        rect.w = rect.w < 0 ? 0 : rect.w > D_8009574C.width - 1 ? D_8009574C.width - 1 : rect.w;
        rect.h = rect.h < 0 ? 0 : rect.h > D_8009574C.height - 1 ? D_8009574C.height - 1 : rect.h;
        rect.x -= env->ofs[0];
        rect.y -= env->ofs[1];
        words[n++] = 0x60000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
        words[n++] = *(unsigned int *)&rect.x;
        words[n++] = *(unsigned int *)&rect.w;
    }
    ((unsigned char *)words)[3] = n - 1;
}

void Gpu_SetDrawEnvBack(DR_ENV *packet, DRAWENV *env)
{
    RECT rect;
    unsigned int *words = (unsigned int *)packet;
    int n;
    words[1] = Gpu_BuildDrawAreaTopLeftCmd(env->clip.x, env->clip.y);
    words[2] = Gpu_BuildDrawAreaBottomRightCmd(
        (short)(env->clip.w + env->clip.x - 1), (short)(env->clip.y + env->clip.h - 1));
    words[3] = Gpu_BuildDrawOffsetCmd(env->ofs[0], env->ofs[1]);
    words[4] = Gpu_BuildDrawModeCmd(env->dfe, env->dtd, env->tpage);
    words[5] = Gpu_BuildTexWindowCmd((GpuTextureWindowRectBytes *)&env->tw);
    words[6] = 0xE6000000;
    n = 7;
    if (env->isbg) {
        rect.x = env->clip.x;
        rect.y = env->clip.y;
        rect.w = env->clip.w;
        rect.h = env->clip.h;
        rect.w = rect.w < 0 ? 0 : rect.w > D_8009574C.width - 1 ? D_8009574C.width - 1 : rect.w;
        rect.h = rect.h < 0 ? 0 : rect.h > D_8009574C.height - 1 ? D_8009574C.height - 1 : rect.h;
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
