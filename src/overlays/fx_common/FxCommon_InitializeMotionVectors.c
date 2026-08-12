#include "fx_common.h"

void func_801938E8(s16 base)
{
    FxCommonMotionVec vector;
    s32 extra[2];
    unsigned int i;

    D_8019C058 = 0;
    for (i = 0; i < 10; i++) {
        func_8018F55C(i << 8, base + 0x40,
                      func_8006EC6C(&D_801D0260, 2), &vector, extra);
        D_8019CAA8[i].x = vector.x << 16;
        D_8019CAA8[i].y = vector.y << 16;
        D_8019CAA8[i].z = vector.z << 16;
    }
}
