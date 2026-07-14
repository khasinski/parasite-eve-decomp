#include "../room_lib/room_lib.h"

void func_8018FA90(void *unused, char *state, void *obj) {
    if (RW16(state, 2) == 0x3A) {
        state[1] = 2;
    }
}
