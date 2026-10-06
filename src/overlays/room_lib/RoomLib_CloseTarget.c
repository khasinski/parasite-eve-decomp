/*
 * Room module class, slot 5: closes the module and, while the field engine
 * is running, its link target. The same function in every room that links
 * it (pe1/room_module.h).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

ROOMLIB_CLOSE_TARGET(RoomLib_CloseTarget)
