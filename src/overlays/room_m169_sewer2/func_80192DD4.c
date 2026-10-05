#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80192E9C(RoomEnt *o);

int func_80192DD4(RoomEnt *o, int arg1, unsigned int op, int arg3, int sp10, int sp14) {
    if (op == 0xA) {
        goto case10;
    }
    if (op >= 0xB) {
        goto high;
    }
    if (op == 4) {
        goto case4;
    }
    goto done;

high:
    if (op == 0x19) {
        goto case25;
    }
    if (op == 0x1C) {
        goto case28;
    }
    goto done;

case25:
    if (arg1 != 1) {
        goto done;
    }
    o->sub.signal = (int *)arg3;
    *(int *)arg3 = arg1;
    goto done;

case4:
    o->pos[0] = arg3;
    o->pos[1] = sp10;
    o->h44 = sp14;
    goto done;

case28:
    o->h48 = arg3;
    o->h46 = sp10;
    goto done;

case10:
    o->t16 = arg3;
    o->sub.cb = (void (*)(void))RoomLib_RearmOnMatch_80192E9C;

done:
    return 0;
}
