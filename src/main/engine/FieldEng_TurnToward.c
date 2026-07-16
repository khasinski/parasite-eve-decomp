/* CC1_FLAGS: -fno-schedule-insns */

typedef short s16;

int FieldEng_TurnToward(int current, int target, int step) {
    register int target_reg asm("$5");
    register int target_copy asm("$8");
    register int diff asm("$5");
    register int abs_diff asm("$7");
    register int step_s16 asm("$2");
    register int delta asm("$3");
    register int result asm("$2");

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
