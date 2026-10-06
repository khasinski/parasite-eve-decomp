/*
 * Room module class, slot 4, in the modules whose close method leaves the
 * target's state alone (RoomLib_ReleaseTarget): once the field engine runs
 * the object (status 3), registers the module's update handlers and spawns
 * its objects; releases the module when either call fails or the object is
 * not running. The lists are the room's (pe1/room_module.h).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

ROOMLIB_START_AT3(RoomLib_RegisterPairedTablesRelease, g_RoomUpdateList,
                  g_RoomInitList, g_RoomSpawnLayout, RoomLib_ReleaseTarget)
