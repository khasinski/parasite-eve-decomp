/* CC1_FLAGS: -fno-schedule-insns */

#include "common.h"

int FieldEng_TurnToward(int current, int target, int step) {
    register int target_reg asm("$5");
    register int target_copy asm("$8");
    register int diff asm("$5");
    int abs_diff;
    int step_s16;
    register int delta asm("$3");
    int result;
    current &= 0xFFF;
    target_reg = target & 0xFFF;
    target_copy = target_reg;
    diff = (s16)(target_reg - current);

    result = diff;
    if (diff < 0) {
        result = -result;
    }
    abs_diff = (s16)result;

    step_s16 = (s16)step;
    delta = step;
    if (step_s16 < abs_diff) {
        goto adjust;
    }
    result = target_copy;
    goto done;

adjust:
    result = abs_diff < 0x801;
    if (diff < 0) {
        delta = -step;
    }
    if (!result) {
        delta = -delta;
    }
    result = delta + current;

done:
    return result & 0xFFF;
}


#include "common.h"
s32 func_800E0024(s32 lhs, s32 rhs) {
    register s32 delta asm("$4");
    register s32 abs_delta asm("$3");

    delta = lhs - rhs;
    if ((delta << 16) >= 0) {
        abs_delta = delta;
    } else {
        abs_delta = -delta;
    }

    if ((short)abs_delta >= 0x801) {
        abs_delta = 0x1000 - abs_delta;
    }

    return (short)abs_delta;
}
