#include "../room_lib/room_lib.h"

void func_80191200(void *unused, unsigned char *state, char *p) {
    register unsigned short v0 asm("$2") = RWU16(p, 0x10);
    register unsigned short v1 asm("$3") = RWU16(p, 0x12);
    register unsigned short v4 asm("$4") = RWU16(p, 0x16);

    v0 += 0x64;
    RWU16(p, 0x10) = v0;
    v0 = RWU16(p, 0x16);
    v1 += v4;
    RWU16(p, 0x12) = v1;
    v1 = RWU16(p, 0x14);
    v0 += 0x32;
    v1 -= 0x10;
    RWU16(p, 0x14) = v1;
    RWU16(p, 0x16) = v0;
    if ((short)v1 < 0) {
        state[1] = 2;
        RWU16(p, 0x14) = 0;
    }
}
