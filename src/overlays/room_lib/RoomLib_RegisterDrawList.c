/*
 * Room module class, slot 3: registers the module's draw handlers with the
 * field engine once it is running. The same function in every room that
 * links it; the list is the room's g_RoomDrawList (pe1/room_module.h).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

ROOMLIB_REGISTER_TABLE(RoomLib_RegisterDrawList, g_RoomDrawList)
