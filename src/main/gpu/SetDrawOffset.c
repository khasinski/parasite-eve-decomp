/* MASPSX_FLAGS: --stack-return-delay */

asm("SetDrawOffset = SetDrawOffset");

typedef signed short s16;

typedef struct {
    unsigned char pad0[3];
    unsigned char code;
    int field4;
    int field8;
} Prim;

typedef struct {
    s16 x;
    s16 y;
} Point;

int Gpu_BuildDrawOffsetCmd(int x, int y);

void SetDrawOffset(Prim *arg0, Point *arg1) {
    arg0->code = 2;
    arg0->field4 = Gpu_BuildDrawOffsetCmd(arg1->x, arg1->y);
    arg0->field8 = 0;
}
