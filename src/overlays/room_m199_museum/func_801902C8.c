#include "../room_lib/room_lib.h"

extern void func_80190370(void);
extern void func_801908FC(RoomLink *l, struct RoomSub *s);

void func_801902C8(RoomEnt *o) {
    struct RoomSub *s = &o->sub;
    signed char c;
    int t;
    unsigned short lo;

    if (RW16(o, 0x80) != 0) {
        func_801908FC(o->link, s);
    }
    c = o->t16;
    if (c >= 0 && c != o->link->variant) {
        return;
    }
    t = o->t17;
    if (t >= 0) {
        RoomLink *l = o->link;
        lo = l->winLo;
        if (t >= l->winHi && t <= lo) {
            o->sub.cb = func_80190370;
        }
    } else {
        o->sub.cb = func_80190370;
    }
}
