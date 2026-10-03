#include "common.h"

extern s8 D_800B0DBB;
extern s8 D_801223F8;

void func_801223EC(s8 enabled) {
    if ((enabled && !D_800B0DBB) || (!enabled && D_800B0DBB)) {
        D_801223F8 = 1;
    }
}
