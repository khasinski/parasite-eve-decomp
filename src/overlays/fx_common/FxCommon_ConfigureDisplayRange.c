#include "fx_common.h"

void func_80191DE8(int extended)
{
    if (extended == 0) {
        D_800B0DCE = 0x10;
        D_800B0DCF = 0x10;
    } else {
        D_800B0DCE = 0x10;
        D_800B0DCF = 0x40;
    }
    D_800B0DD0 = 0;
    D_800B0DD2 = 0xFFF;
}
