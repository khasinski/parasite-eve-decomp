#include "../room_lib/room_lib.h"

typedef struct RoomLocalTimer {
    char pad0[0x64];
    unsigned short reload;
    short count;
    short request;
} RoomLocalTimer;

void func_8018F71C(int a0, unsigned char *state, RoomLocalTimer *timer) {
    register unsigned char *statep asm("s0") = state;
    short count = timer->count;
    register RoomLocalTimer *p asm("a1") = timer;
    if (count != 0) {
        timer->count = count - 1;
    }
    if (p->request == 1) {
        p->request = 0;
        if (p->count == 0) {
            p->count = p->reload;
            func_800C6C18();
        }
    }
    if (func_800C2B68() == 1) {
        statep[1] = 2;
    }
}
