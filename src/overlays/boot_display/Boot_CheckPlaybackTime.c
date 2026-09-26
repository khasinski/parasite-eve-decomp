#include "pe1/boot_display.h"

void Boot_CheckPlaybackTime(void) {
    volatile s32 *time;
    if ((func_80073A44(-1) & 31) == 0) {
        time = &D_80172CF0;
        if (*time > D_80172CEC) {
            func_800870F0(0);
            D_80172CE4 = 1;
        } else if (*time < D_80172CE8 - 300) {
            func_801258EC(200, 27);
        } else {
            func_8007EE84(17, 0, D_80125AF0, 0);
        }
    }
}
