/*
 * The module class of scene_e08's second module (emitter and fade slots,
 * the player orb and its ring sprites): the seven methods its class table
 * lists in front of the module. The registration and start methods act
 * whatever the field engine's status. The script and the lists are named
 * in the symbol file (scene_e08_modules.h).
 */
#include "../room_lib/room_lib.h"
#include "scene_e08_modules.h"

ROOMLIB_RETURN_ZERO(SceneE08_OrbNop0)

ROOMLIB_PLANT_TABLE(SceneE08_OrbPlantScript, g_SceneE08OrbScript)

ROOMLIB_SPAWN6(SceneE08_OrbSpawn6)

ROOMLIB_REGISTER_TABLE_ANY(SceneE08_OrbRegister, g_SceneE08OrbDrawList)

ROOMLIB_START_ANY(SceneE08_OrbStart, g_SceneE08OrbUpdateList,
                  g_SceneE08OrbInitList, g_SceneE08OrbSpawnLayout,
                  SceneE08_OrbClose)

ROOMLIB_CLOSE_TARGET(SceneE08_OrbClose)

ROOMLIB_RETURN_ZERO(SceneE08_OrbNop6)
