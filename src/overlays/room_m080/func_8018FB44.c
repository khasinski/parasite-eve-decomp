#include "../room_lib/room_lib.h"

void func_8018FB44(void *arg0, char *state, char *obj) {
    register char *p asm("a0") = obj;
    if ((unsigned short)(RWU16(state, 2) - 1) < 7U) {
        RWU16(obj, 0x12) += 0x10;
    }
    if ((unsigned short)(RWU16(state, 2) - 0x15) < 0x1DU) {
        RWU16(obj, 0x10) += 0x28;
    }
    if (RW16(state, 2) >= 0x33 && RW16(p, 0x12) >= 0x11) {
        RWU16(p, 0x12) -= 0x10;
    }
    if (RW16(state, 2) == 0x3A) {
        RW8(state, 1) = 2;
    }
}
