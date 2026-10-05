#include "../room_lib/room_lib.h"

void func_8018FF10(void *arg0, char *state, char *obj) {
    if (RW16(state, 2) < 0x10) {
        RWU16(obj, 0x10) += 0xF0;
        if (RW16(state, 2) < 0x10) {
            RWU16(obj, 0x12) -= 8;
        }
    }
    if (RW16(state, 2) == 0x10) {
        RW8(state, 1) = 2;
    }
}
