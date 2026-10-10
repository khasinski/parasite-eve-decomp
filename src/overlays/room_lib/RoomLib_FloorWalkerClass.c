/* MASPSX_FLAGS: --expand-div */
/*
 * The floor walker actor class driven by RoomLib_HandlerG: a sixth class
 * that 8 rooms (room_m145, m152, m153, m154, m332, m399, m401, m402) link
 * directly after the room library (RoomLib_ActorClasses.c), listed in the
 * same room class table with the same seven-method layout (no-op, Init,
 * Configure, no-op, Update, ..., Release, no-op).
 */
#include "room_lib.h"

int RoomLib_HandlerGNop0(void) {
    return 0;
}
int RoomLib_InitHandlerG(RoomEnt *o) {
    unsigned int m = 0x10002;
    RoomLink *l = o->link;
    o->t16 = -1;
    o->t17 = -1;
    o->t18 = -1;
    o->t19 = 3;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    RW16(o, 0x32) = 0;
    RW16(o, 0x36) = 0;
    o->sub.cb = RoomLib_HandlerG;
    RW32(l, 0x98) |= m;
    RWU16(l, 0x250) |= 0x400;
    return 0;
}

int RoomLib_ConfigureHandlerG(RoomEnt *o, int mode, unsigned int op,
                              int arg0, int arg1, int arg2) {
    char *p = (char *)o + 0xC;

    if (op == 4) {
        goto case4;
    }
    if (op < 5) {
        if (op == 0) {
            goto case0;
        }
        return 0;
    }
    if (op == 12) {
        goto case12;
    }
    if (op != 25) {
        return 0;
    }
    if (mode != 1) {
        return 0;
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
            return 0;
        }
        RoomLib_ReleaseHandlerG(o);
    } else {
        RoomLib_ReleaseHandlerG(o);
    }
    goto ret;

case12:
    RW32(o, 0x24) = arg0;
    RW32(o, 0x2C) = arg1;
    RW16(o, 0x34) = arg2;

ret:
    return 0;
}

int RoomLib_HandlerGNop3(void) {
    return 0;
}

#define ROOMLIB_UPDATE_TRANSFORM_SCALE_NAME RoomLib_UpdateHandlerG
#include "RoomLib_UpdateTransformScale.inc"

void RoomLib_HandlerG(RoomEnt *o) {
    RoomEnt *self = o;
    RoomLink *src = (RoomLink *)RW32(self, 0x1C);
    RoomLink *dst = self->link;
    dst->pos[0] = RW32(src->p238, 0x94) << 16;
    dst->pos[1] = RW32(src->p238, 0x98) << 16;
    dst->pos[2] = RW32(src->p238, 0x9C) << 16;
    *(RoomLibPacked8 *)((char *)dst + 0x38) = *(RoomLibPacked8 *)((char *)src + 0x38);
    if ((src->variant == 7) && (src->winLo >= 3) && (src->winHi < 3)) {
        self->sub.cb = RoomLib_HandlerGFloorWalker;
    }
    if (src->target != 0) {
        if (func_8003010C(src, 0x2C) <= 0) {
            func_80030220(dst, 0x2D, 0);
            func_80030220(dst, 0x2C, 0);
            func_80030220(dst, 0x5F, 0);
            RoomLib_ReleaseHandlerG(self);
        }
    } else {
        RoomLib_ReleaseHandlerG(self);
    }
}

#define ROOMLIB_FLOOR_WALKER_FUNC RoomLib_HandlerGFloorWalker
#include "RoomLib_FloorWalker.inc"

int RoomLib_ReleaseHandlerG(RoomEnt *o) {
    int *p = o->sub.signal;
    o->state = 4;
    if (p != 0) {
        *p = 0;
    }
    return 0;
}
int RoomLib_HandlerGNop6(void) {
    return 0;
}

