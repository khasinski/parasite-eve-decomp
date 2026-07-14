#include "../room_lib/room_lib.h"

void func_80190128(void *unused, unsigned char *state, char *p) {
    RW8(p, 2)++;

    if ((signed char)RW8(p, 2) >= 0x15) {
        state[1] = 2;
    }

    if (RW16(p, 4) >= 9) {
        RW16(p, 4) -= 8;
    }
}
