#include "../room_lib/room_lib.h"

void func_8018FAEC(void *unused0, void *unused1, char *state) {
    char *obj = (char *)func_800C2B50();
    register int tmp asm("$3");
    register int cst asm("$2");

    tmp = RW32(obj, 0x18);
    RW16(state, 0) = tmp;
    tmp = RW32(obj, 0x1C);
    RW16(state, 2) = tmp;
    tmp = RW32(obj, 0x20);
    cst = 0x400;
    RW16(state, 0xA) = 0;
    RW16(state, 8) = cst;
    RW16(state, 4) = tmp;
}
