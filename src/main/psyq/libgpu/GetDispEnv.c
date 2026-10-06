/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ part: GetDispEnv, GetODE, SetTexWindow, SetDrawArea,
 * SetDrawOffset, SetPriority, SetDrawStp and SetDrawMode.
 * The other SYS.OBJ functions are in neighbouring units because their
 * reconstructions need different compiler options or conflicting
 * declarations.
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

int Gpu_BuildTexWindowCmd(int arg0);

int Gpu_BuildDrawAreaTopLeftCmd(int x, int y);
int Gpu_BuildDrawAreaBottomRightCmd(int x, int y);

typedef signed short s16_1;

typedef struct {
    s16_1 x;
    s16_1 y;
} Point;

int Gpu_BuildDrawOffsetCmd(int x, int y);

void SetTexWindow(GpuCmdPacket *arg0, int arg1) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildTexWindowCmd(arg1);
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

int Gpu_BuildDrawModeCmd(int arg0, int arg1, int arg2);
int Gpu_BuildTexWindowCmd(int arg0);

void SetDrawMode(GpuCmdPacket *arg0, int arg1, int arg2, int arg3, int arg4) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildDrawModeCmd(arg1, arg2, arg3 & 0xFFFF);
    arg0->field8 = Gpu_BuildTexWindowCmd(arg4);
}
