#include "../room_lib/room_lib.h"

extern char *D_8009D20C;
extern int RoomLib_Set4ClearSignal_80192CBC(RoomEnt *o);

int func_8019255C(RoomEnt *o, int mode, unsigned int op, int arg0, int arg1, int arg2) {
    char *p = (char *)o + 0xC;

    if (op == 4) {
        goto case4;
    }
    if (op < 5) {
        if (op == 0) {
            goto case0;
        }
        goto ret;
    }
    if (op == 12) {
        goto case12;
    }
    if (op != 25) {
        goto ret;
    }
    if (mode != 1) {
        goto ret;
    }
    RW32(o, 0x10) = arg0;
    RW32(arg0, 0) = mode;
    goto ret;

case4:
    RW32(o, 0x20) = arg0;
    goto ret;

case0:
    RW32(o, 0x1C) = (int)D_8009D20C;
    if (D_8009D20C != 0) {
        do {
            char *cur = (char *)RW32(p, 0x10);
            if ((unsigned char)cur[0xC] == arg0 && (unsigned char)cur[0xD] == arg1) {
                if ((RW32(cur, 0x98) & 0x10) == 0) {
                    break;
                }
            }
            RW32(p, 0x10) = RW32((char *)RW32(p, 0x10), 4);
        } while (RW32(p, 0x10) != 0);
        if (RW32(p, 0x10) != 0) {
            goto ret;
        }
        RoomLib_Set4ClearSignal_80192CBC(o);
    } else {
        RoomLib_Set4ClearSignal_80192CBC(o);
    }
    goto ret;

case12:
    RW32(o, 0x24) = arg0;
    RW32(o, 0x2C) = arg1;
    RW16(o, 0x34) = arg2;

ret:
    return 0;
}
