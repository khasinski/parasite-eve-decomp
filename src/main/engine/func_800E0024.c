typedef int s32;

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
