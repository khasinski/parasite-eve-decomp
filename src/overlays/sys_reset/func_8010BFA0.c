#include "common.h"
#include "pe1/sys_reset.h"

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
