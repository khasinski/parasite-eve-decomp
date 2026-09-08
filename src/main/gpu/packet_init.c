/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_gpu.h"

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

int Gpu_BuildDrawModeCmd(int arg0, int arg1, int arg2);
int Gpu_BuildTexWindowCmd(int arg0);

void Gpu_InitDrawModeTexWindowPacket(GpuCmdPacket *arg0, int arg1, int arg2, int arg3, int arg4) {
    arg0->u0.head.code = 2;
    arg0->field4 = Gpu_BuildDrawModeCmd(arg1, arg2, arg3 & 0xFFFF);
    arg0->field8 = Gpu_BuildTexWindowCmd(arg4);
}
