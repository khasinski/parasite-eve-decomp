#include "room_lib.h"

void func_80191200(void *arg0, char *arg1, char *arg2) {
    *(unsigned short *)(arg2 + 0x10) += 0x64;
    *(unsigned short *)(arg2 + 0x12) += *(unsigned short *)(arg2 + 0x16);
    *(unsigned short *)(arg2 + 0x16) += 0x32;
    *(unsigned short *)(arg2 + 0x14) -= 0x10;

    if (*(short *)(arg2 + 0x14) < 0) {
        arg1[1] = 2;
        *(unsigned short *)(arg2 + 0x14) = 0;
    }
}


void func_80191250(void *unused0, void *unused1, char *state) {
    RoomClock *owner;
    unsigned int i;

    owner = func_800C2B50();
    RWU16(state, 0xEA) = 0x80;
    RW32(state, 0xE0) = 0xFFEC0000;
    RW32(state, 0xE4) = RW32((char *)owner, 0x58) << 8;

    for (i = 0; i < 8; i++) {
        RWU16(state, 0x88 + i * 2) = 0;
        RW32(state, 0xA8 + i * 4) = 0;
        RW32(state, 0x0C + i * 0x10) = RW16(state, 2) << 16;
        RW8(state + i, 0xC8) = 1;
        RWU16(state, 0xD0 + i * 2) = 0x80;
    }
}
