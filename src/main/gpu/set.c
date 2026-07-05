/* MASPSX_FLAGS: --stack-return-delay */

asm("GetDispEnv = GetDispEnv");

extern void *memcpy(void *dest, const void *src, unsigned int n);
extern char g_GpuActiveDispEnv[];

#include "pe1/gpu_callbacks.h"

extern GpuCallbacks *g_GpuCallbacks;

asm("SetTexWindow = SetTexWindow");

typedef struct {
    unsigned char pad0[3];
    unsigned char code;
    int field4;
    int field8;
} Prim;

int Gpu_BuildTexWindowCmd(int arg0);

asm("SetDrawArea = SetDrawArea");

typedef signed short s16;
typedef unsigned short u16;

typedef struct {
    s16 x;
    s16 y;
    u16 w;
    u16 h;
} Rect;

int Gpu_BuildDrawAreaTopLeftCmd(int x, int y);
int Gpu_BuildDrawAreaBottomRightCmd(int x, int y);

asm("SetDrawOffset = SetDrawOffset");

typedef signed short s16_1;

typedef struct {
    s16_1 x;
    s16_1 y;
} Point;

int Gpu_BuildDrawOffsetCmd(int x, int y);

asm("SetDrawMask = SetDrawMask");

void Gpu_InitDrawMaskSetPacket(void *arg0, int arg1);

void *GetDispEnv(void *arg0) {
    void *(*fn)(void *, const void *, unsigned int);

    fn = memcpy;
    fn(arg0, g_GpuActiveDispEnv, 0x14);
    return arg0;
}

int Gpu_PollCallback(void) {
    return g_GpuCallbacks->callback() < 0;
}

void SetTexWindow(Prim *arg0, int arg1) {
    arg0->code = 2;
    arg0->field4 = Gpu_BuildTexWindowCmd(arg1);
    arg0->field8 = 0;
}

void SetDrawArea(Prim *arg0, Rect *arg1) {
    arg0->code = 2;
    arg0->field4 = Gpu_BuildDrawAreaTopLeftCmd(arg1->x, arg1->y);
    arg0->field8 = Gpu_BuildDrawAreaBottomRightCmd((s16)(arg1->x + arg1->w - 1), (s16)(arg1->y + arg1->h - 1));
}

void SetDrawOffset(Prim *arg0, Point *arg1) {
    arg0->code = 2;
    arg0->field4 = Gpu_BuildDrawOffsetCmd(arg1->x, arg1->y);
    arg0->field8 = 0;
}

void SetDrawMask(void *arg0, int arg1, int arg2) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) =
        (arg1 ? 0xE6000002 : 0xE6000000) | (arg2 != 0);
    *(int *)((char *)arg0 + 8) = 0;
}

void Gpu_InitDrawMaskSetPacket(void *arg0, int arg1) {
    *(char *)((char *)arg0 + 3) = 2;
    *(unsigned int *)((char *)arg0 + 4) = arg1 ? 0xE6000001 : 0xE6000000;
    *(int *)((char *)arg0 + 8) = 0;
}
