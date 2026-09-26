#include "pe1/boot_display.h"

s32 Boot_StepPlaybackTransfer(u8 mode) {
    volatile u16 *index;
    u16 next;
    if (func_8007F72C() == 1 && func_8007F7A8() != D_800B0DD4)
        func_800719E4(1);
    if (mode == 1) {
        index = &D_80172CF8;
        func_80080AE4(D_80172D00 + (*index << 11), 512);
        next = (*index + 1) & 7;
        *index = next;
    } else {
        D_80172CFA = D_80172CFA + 1;
        next = D_80172CFA;
    }
    return next;
}
