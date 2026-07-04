/* MASPSX_FLAGS: --stack-return-delay */

asm("SetTexWindow = SetTexWindow");

typedef struct {
    unsigned char pad0[3];
    unsigned char code;
    int field4;
    int field8;
} Prim;

int Gpu_BuildTexWindowCmd(int arg0);

void SetTexWindow(Prim *arg0, int arg1) {
    arg0->code = 2;
    arg0->field4 = Gpu_BuildTexWindowCmd(arg1);
    arg0->field8 = 0;
}
