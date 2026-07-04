/* MASPSX_FLAGS: --stack-return-delay */

typedef struct {
    unsigned char pad0[3];
    unsigned char code;
    int field4;
    int field8;
} Prim;

int Gpu_BuildDrawModeCmd(int arg0, int arg1, int arg2);
int Gpu_BuildTexWindowCmd(int arg0);

void Gpu_InitDrawModeTexWindowPacket(Prim *arg0, int arg1, int arg2, int arg3, int arg4);

void Gpu_InitDrawModeTexWindowPacket(Prim *arg0, int arg1, int arg2, int arg3, int arg4) {
    arg0->code = 2;
    arg0->field4 = Gpu_BuildDrawModeCmd(arg1, arg2, arg3 & 0xFFFF);
    arg0->field8 = Gpu_BuildTexWindowCmd(arg4);
}
