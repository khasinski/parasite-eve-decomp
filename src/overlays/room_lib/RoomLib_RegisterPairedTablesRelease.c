/*
 * Room module class, slot 4, in the modules whose close method leaves the
 * target's state alone (RoomLib_ReleaseTarget): once the field engine runs
 * the object (status 3), registers the module's update handlers and spawns
 * its objects; releases the module when either call fails or the object is
 * not running. The lists are the room's (pe1/room_module.h).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

int RoomLib_RegisterPairedTablesRelease(void *o) {
    int result;

    if (FieldEng_GetStatus(o) == 3) {
        result = func_800C251C(o, g_RoomUpdateList);
        result |= func_800C2758(o, g_RoomInitList, g_RoomSpawnLayout);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_ReleaseTarget(o);
    }

    return 0;
}
