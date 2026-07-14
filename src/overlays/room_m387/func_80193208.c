#include "../room_lib/room_lib.h"

extern int func_80193250(RoomEnt *o);

void func_80193208(RoomEnt *o) {
    RoomLink *l = o->link->link18C;

    if ((unsigned int)l->winHi >= (unsigned int)(RW8(l, 0xF) - 1)) {
        func_80193250(o);
    }
}
