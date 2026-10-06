/*
 * The module class of scene_e08's fourth module (effect palettes, the
 * layered sprite and the arm glow): the seven methods its class table
 * lists in front of the module. The registration and start methods act
 * whatever the field engine's status. The script and the lists are named
 * in the symbol file (scene_e08_modules.h).
 */
#include "../room_lib/room_lib.h"
#include "scene_e08_modules.h"

ROOMLIB_RETURN_ZERO(SceneE08_LayeredSpriteNop0)

ROOMLIB_PLANT_TABLE(SceneE08_LayeredSpritePlantScript, g_SceneE08LayeredSpriteScript)

ROOMLIB_SPAWN6(SceneE08_LayeredSpriteSpawn6)

ROOMLIB_REGISTER_TABLE_ANY(SceneE08_LayeredSpriteRegister, g_SceneE08LayeredSpriteDrawList)

ROOMLIB_START_ANY(SceneE08_LayeredSpriteStart, g_SceneE08LayeredSpriteUpdateList,
                  g_SceneE08LayeredSpriteInitList, g_SceneE08LayeredSpriteSpawnLayout,
                  SceneE08_LayeredSpriteClose)

ROOMLIB_CLOSE_TARGET(SceneE08_LayeredSpriteClose)

ROOMLIB_RETURN_ZERO(SceneE08_LayeredSpriteNop6)
