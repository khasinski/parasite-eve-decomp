/* MASPSX_FLAGS: --stack-return-delay */
asm(".globl SetDrawArea");
asm("SetDrawArea = SetDrawArea");

typedef signed short s16;
typedef unsigned short u16;

typedef struct {
    unsigned char pad0[3];
    unsigned char code;
    int field4;
    int field8;
} Prim;

typedef struct {
    s16 x;
    s16 y;
    u16 w;
    u16 h;
} Rect;

int Gpu_BuildDrawAreaTopLeftCmd(int x, int y);
int Gpu_BuildDrawAreaBottomRightCmd(int x, int y);

void SetDrawArea(Prim *arg0, Rect *arg1) {
    arg0->code = 2;
    arg0->field4 = Gpu_BuildDrawAreaTopLeftCmd(arg1->x, arg1->y);
    arg0->field8 = Gpu_BuildDrawAreaBottomRightCmd((s16)(arg1->x + arg1->w - 1), (s16)(arg1->y + arg1->h - 1));
}
