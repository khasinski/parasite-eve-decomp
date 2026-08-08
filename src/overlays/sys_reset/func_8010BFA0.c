#include "common.h"
extern void func_8010C1EC(s32 *arg0, u32 arg1);

void func_8010BFA0(s32 *arg0, s32 arg1) {
    if (arg1 & 1) {
        *arg0 &= ~0x08000000;
    } else {
        *arg0 |= 0x08000000;
    }

    if (arg1 & 2) {
        *arg0 |= 0x02000000;
    } else {
        *arg0 &= ~0x02000000;
    }

    func_8010C1EC(arg0, *(u16 *)arg0);
}
