#include "common.h"

extern s8 D_800B0DBB;
extern s8 D_801223F8;

void func_801223EC(u32 enabled) {
again:
    asm("" : "=r"(enabled) : "0"(enabled));
    enabled <<= 24;
    if (enabled) {
        if (!D_800B0DBB) {
            goto store;
        }
        goto again;
    } else if (!D_800B0DBB) {
        return;
    }

store:
    D_801223F8 = 1;
}
