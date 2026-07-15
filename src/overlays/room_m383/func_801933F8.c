#include "../room_lib/room_lib.h"

typedef unsigned char u8;
typedef unsigned short u16;

void func_801933F8(void *arg0, u8 *state, u8 *obj) {
    register u8 *pos asm("$6");
    register u8 *timer asm("$7");
    register unsigned int i asm("$8");
    int value;

    pos = obj;
    i = 0;
    timer = obj;
    do {
        RWU16(pos, 0x30) += RWU16(pos, 0x50);
        RWU16(pos, 0x32) += RWU16(pos, 0x52);
        RWU16(pos, 0x34) += RWU16(pos, 0x54);

        value = RWU16(timer, 0x78) - 8;
        RWU16(timer, 0x78) = value;
        if ((short)value < 0) {
            RWU16(timer, 0x78) = 0;
        }

        timer += 2;
        i++;
        pos += 8;
    } while (i < 4);

    if (RW16(state, 2) >= 0x3D) {
        RW8(state, 1) = 2;
    }
}
