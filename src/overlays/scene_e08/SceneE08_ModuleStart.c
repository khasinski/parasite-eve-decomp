/*
 * Registration and start methods of scene_e08's first module, between the
 * shared RoomLib_Spawn6 and RoomLib_CloseTarget in its class. Unlike
 * RoomLib_RegisterDrawList and RoomLib_RegisterPairedTables they act
 * whatever the field engine's status.
 */
#include "../room_lib/room_lib.h"
#include "scene_e08_modules.h"

ROOMLIB_REGISTER_TABLE_ANY(SceneE08_RegisterDrawList, g_RoomDrawList)

ROOMLIB_START_ANY(SceneE08_StartModule, g_RoomUpdateList, g_RoomInitList,
                  g_RoomSpawnLayout, RoomLib_CloseTarget)
