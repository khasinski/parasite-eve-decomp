#include "../room_lib/room_lib.h"

void func_8018FC34(void *unused, unsigned char *state, char *p) {
    register unsigned short v0 asm("$2") = RWU16(p, 0x10);
    register unsigned short v1 asm("$3") = RWU16(p, 0x12);

    v0 += 0xC8;
    RWU16(p, 0x10) = v0;
    v0 = RWU16(p, 0x14);
    v1 -= 0x10;
    RWU16(p, 0x12) = v1;
    v0 += 1;
    RWU16(p, 0x14) = v0;
    if ((short)v0 >= 8) {
        state[1] = 2;
    }
    if (RW16(p, 0x12) < 0) {
        RWU16(p, 0x12) = 0;
    }
}
