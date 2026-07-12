#include "../room_lib/room_lib.h"

extern int func_800DFB78(void);
extern void func_8018FECC(RoomEnt *o);

int func_8018F534(RoomEnt *o) {
    switch (func_800DFB78()) {
    case 0:
        if (o->link->variant < 2) {
            return 0;
        }
        ((void (*)(RoomEnt *))o->sub.cb)(o);
        return 0;
    case 1:
        func_8018FECC(o);
    case 2:
        return 0;
    }
    return 0;
}
