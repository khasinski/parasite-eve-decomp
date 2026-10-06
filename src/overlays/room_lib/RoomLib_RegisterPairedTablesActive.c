/*
 * Room module class, slot 4, in the modules that start only while the field
 * engine runs the object (status 3) rather than from status 2 on
 * (RoomLib_RegisterPairedTables): registers the module's update handlers
 * and spawns its objects; closes the module when either call fails or the
 * object is not running. The same function in every room that links it;
 * the lists are the room's (pe1/room_module.h).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

ROOMLIB_START_AT3(RoomLib_RegisterPairedTablesActive, g_RoomUpdateList,
                  g_RoomInitList, g_RoomSpawnLayout, RoomLib_CloseTarget)
