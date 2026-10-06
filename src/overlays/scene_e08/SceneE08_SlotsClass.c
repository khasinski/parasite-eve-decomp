/*
 * The module class of scene_e08's third module (moving and particle slots,
 * the glow model and the limb beams): the seven methods its class table
 * lists in front of the module. The registration and start methods act
 * whatever the field engine's status. The script and the lists are named
 * in the symbol file (scene_e08_modules.h).
 */
#include "../room_lib/room_lib.h"
#include "scene_e08_modules.h"

ROOMLIB_RETURN_ZERO(SceneE08_SlotsNop0)

ROOMLIB_PLANT_TABLE(SceneE08_SlotsPlantScript, g_SceneE08SlotsScript)

ROOMLIB_SPAWN6(SceneE08_SlotsSpawn6)

ROOMLIB_REGISTER_TABLE_ANY(SceneE08_SlotsRegister, g_SceneE08SlotsDrawList)

ROOMLIB_START_ANY(SceneE08_SlotsStart, g_SceneE08SlotsUpdateList,
                  g_SceneE08SlotsInitList, g_SceneE08SlotsSpawnLayout,
                  SceneE08_SlotsClose)

ROOMLIB_CLOSE_TARGET(SceneE08_SlotsClose)

ROOMLIB_RETURN_ZERO(SceneE08_SlotsNop6)
