#include "common.h"

extern u16 D_80199604;
extern u16 D_8019960A, D_8019960C, D_80199606, D_80199608;
extern u8 D_80199600, D_80199601, D_80199602;
extern u8 D_801995FC, D_801995FD, D_801995FE;
extern u8 D_80198C90[];
void func_800C4E50(void *);

void func_801923D8(void *unused0, void *unused1, u8 *effect) {
    u8 *mid = (u8 *)&D_80199604;
    u8 *source;
    u8 *base;
    u8 *third;
    int intensity = 0x80;
    register int span asm("$19");
    asm("" : "=r"(mid) : "0"(mid));

    base = mid - 12;
    asm ("" : : "r"(base));
    *(u16 *)(effect + 8) = 0;
    *(u16 *)(effect + 10) = 0;
    *(u16 *)(effect + 12) = 0;
    *(u16 *)(effect + 18) = 0;

    *(u16 *)mid = 0x20;
    D_8019960A = 0x1F4;
    asm volatile("" : : "r"(intensity));
    D_80199600 = 0xF0;
    D_80199601 = 0xC8;
    span = 0x960;
    D_8019960C = intensity;
    D_80199602 = 0x78;
    asm volatile("" : "=r"(source) : "0"(D_80198C90));
    D_801995FC = 0;
    D_801995FD = 0;
    D_801995FE = 0;
    D_80199606 = span;
    D_80199608 = 0;
    *(u8 **)base = source;
    func_800C4E50(base);

    *(u16 *)(mid + 24) = 0x20;
    *(u16 *)(mid + 30) = 0x5DC;
    *(u16 *)(mid + 32) = intensity;
    mid[20] = 0;
    mid[21] = 0;
    mid[22] = 0;
    mid[16] = 0xFF;
    mid[17] = 0xFF;
    mid[18] = 0xFF;
    *(u16 *)(mid + 26) = span;
    *(u16 *)(mid + 28) = 0;
    *(u8 **)(mid + 12) = source + 0x200;
    func_800C4E50(mid + 12);

    third = mid + 36;
    asm ("" : : "r"(third));
    *(u16 *)(mid + 48) = 0x20;
    *(u16 *)(mid + 54) = 0x5DC;
    *(u16 *)(mid + 56) = intensity;
    mid[44] = 0;
    mid[45] = 0;
    mid[46] = 0;
    mid[40] = 0xFF;
    mid[41] = 0xFF;
    mid[42] = 0xFF;
    *(u16 *)(mid + 50) = span;
    *(u16 *)(mid + 52) = 0;
    source += 0x400;
    *(u8 **)third = source;
    func_800C4E50(third);
}
