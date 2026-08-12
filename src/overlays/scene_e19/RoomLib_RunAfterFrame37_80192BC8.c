#include "../room_lib/room_lib.h"

void RoomLib_RunAfterFrame37_80192BC8(RoomEnt *o)
{
    if (o->link->winLo >= 0x26) {
        func_80192BFC();
    }
}
