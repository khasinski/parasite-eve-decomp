#include "pe1/akao/pos.h"

typedef int s32;

extern s32 Akao_Calc3DPan(AkaoPackedRect3 *rect, s32 *out1, s32 *out2);
int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4);

s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, AkaoPosCoord arg3, AkaoPosCoord arg4, AkaoPosCoord arg5) {
    AkaoPackedRect3 rect;
    s32 out1;
    s32 out2;

    rect.x = arg3;
    rect.y = arg4;
    rect.z = arg5;

    Akao_Calc3DPan(&rect, &out1, &out2);
    return Akao_SendTableCommand(arg0, arg1, arg2, out1, out2);
}
