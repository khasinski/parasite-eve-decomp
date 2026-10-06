/*
 * Room module class, slot 4, in the modules that start only once their
 * link is past variant 1 or its target's flag at +0xAC is set: from field
 * engine status 2 on, registers the module's update handlers and spawns its
 * objects when the gate is open; closes the module when either call fails
 * or the engine is not running. The lists are the room's
 * (pe1/room_module.h).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

int RoomLib_RegisterPairedTablesGated(void *o) {
    char *entry;
    int result = 0;

    if ((unsigned int)FieldEng_GetStatus(o) >= 2) {
        entry = *(char **)((char *)o + 8);
        if ((unsigned char)entry[0xE] >= 2 || *(unsigned char *)(*(char **)entry + 0xAC) != 0) {
            result = func_800C251C(o, g_RoomUpdateList);
            result |= func_800C2758(o, g_RoomInitList, g_RoomSpawnLayout);
        }
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget(o);
    }

    return 0;
}
