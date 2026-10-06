#include "../room_lib/room_lib.h"

int func_8018F1B0(RoomEnt *o) {
    o->state = 4;
    if ((unsigned int)FieldEng_GetStatus(o) >= 2) {
        RoomRenderNode *target = o->link->target;
        target->flags &= 0xC0FFFFFF;
    }
    return 0;
}
