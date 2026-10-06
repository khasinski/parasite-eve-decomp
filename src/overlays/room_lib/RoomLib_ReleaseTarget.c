/*
 * Room module class, slot 5, in the modules that close without marking
 * their link target closed: closes the module and, while the field engine
 * is running, clears the target's owner bits (compare RoomLib_CloseTarget).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

int RoomLib_ReleaseTarget(RoomEnt *o) {
    o->state = 4;
    if ((unsigned int)FieldEng_GetStatus(o) >= 2) {
        RoomRenderNode *target = o->link->target;
        target->flags &= 0xC0FFFFFF;
    }
    return 0;
}
