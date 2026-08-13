#include "common.h"

#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"

int Gpu_BuildTexWindowCmd(int arg0);

typedef struct {
    s16 x;
    s16 y;
    u16 w;
    u16 h;
} Rect;

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

void SetDrawArea(GpuCmdPacket *arg0, Rect *arg1) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildDrawAreaTopLeftCmd(arg1->x, arg1->y);
    arg0->field8 = Gpu_BuildDrawAreaBottomRightCmd((s16)(arg1->x + arg1->w - 1), (s16)(arg1->y + arg1->h - 1));
}

void SetDrawOffset(GpuCmdPacket *arg0, Point *arg1) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildDrawOffsetCmd(arg1->x, arg1->y);
    arg0->field8 = 0;
}

void SetDrawMask(void *arg0, int arg1, int arg2) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) =
        (arg1 ? 0xE6000002 : 0xE6000000) | (arg2 != 0);
    *(int *)((char *)arg0 + 8) = 0;
}
