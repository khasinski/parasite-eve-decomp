#include "fx_common.h"

void func_80192740(void)
{
    if (D_8019C1F0 == 1) {
        u32 *dst = D_801EA598;
        FxCommonBuffer *owner = D_8019C9C0;
        FxCommonBuffer *sentinel = (FxCommonBuffer *)&D_8019C1F8;
        u32 lowMask;
        u32 *base;
        u32 highMask;
        u32 *src1;
        u32 *src2;
        u32 destWord;
        u32 lowWord;
        FxCommonAddress address;

        if (owner != sentinel)
            dst += 9;
        lowMask = 0xFFFFFF;
        base = D_801EA598;
        highMask = 0xFF000000;
        src1 = (u32 *)((volatile FxCommonBuffer *)owner)->allocation;
        destWord = *dst;
        lowWord = src1[0xFFF];
        destWord &= highMask;
        lowWord &= lowMask;
        destWord |= lowWord;
        *dst = destWord;
        src2 = (u32 *)((volatile FxCommonBuffer *)owner)->allocation;
        if (owner != sentinel)
            base += 9;
        address.pointer = base;
        lowWord = address.word & lowMask;
        src2[0xFFF] = (src2[0xFFF] & highMask) | lowWord;
    }
    if (D_8019C02C != 0)
        D_8019C02C = func_80193B5C(D_8019C02C);
}
