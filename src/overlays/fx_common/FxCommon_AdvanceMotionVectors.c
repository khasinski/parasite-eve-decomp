#include "fx_common.h"

void func_80193AB0(void)
{
    FxCommonMotionLane *x;
    FxCommonMotionLane *y;
    FxCommonMotionLane *z;
    FxCommonMotionLane *end;
    FxCommonMotionByte *base;
    int sourceOffset;

    if (D_8019C058 <= 0) {
        return;
    }

    D_8019C058--;
    base = (FxCommonMotionByte *)D_8019CAB0;
    z = D_8019CAB0;
    y = (FxCommonMotionLane *)(base - 4);
    x = (FxCommonMotionLane *)(base - 8);
    sourceOffset = 0;
    end = (FxCommonMotionLane *)(base + 0xA0);
    do {
        x->value += *(s32 *)(D_801EA268 + sourceOffset);
        y->value += *(s32 *)(D_801EA26C + sourceOffset);
        z->value += *(s32 *)(D_801EA270 + sourceOffset);
        x++;
        y++;
        z++;
        sourceOffset += 0x10;
    } while (z < end);
}
