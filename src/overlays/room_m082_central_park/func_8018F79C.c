#include "../room_lib/room_lib.h"

void func_8018F79C(int a0, int a1, short *out) {
    char *ctx = (char *)func_800C2B50();

    out[0] = RW32(ctx, 0x58);
    out[1] = RW32(ctx, 0x5C);
    out[2] = RW32(ctx, 0x60);
    out[4] = RW32(ctx, 0x38);
    out[5] = RW32(ctx, 0x3C);
    out[6] = RW32(ctx, 0x40);
    out[9] = 0;
    out[8] = 0x400;
}
