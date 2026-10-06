/*
 * The orbit effect set's module class: the seven methods the class table
 * lists in front of the set (RoomFx_OrbitEffectSet.c). The registration and
 * the start method act only while the field engine runs the object
 * (status 3).
 *
 * room_m187 links the class as its only module; room_m186, m385, m388 and
 * m389 link it after the twelve-element effect's module. The script and the
 * lists are each room's own data (pe1/room_orbit_set.h).
 */
#include "room_lib.h"
#include "pe1/room_orbit_set.h"

ROOMLIB_RETURN_ZERO(RoomFx_OrbitSetNop0)

ROOMLIB_PLANT_TABLE(RoomFx_OrbitSetPlantScript, g_RoomOrbitSetScript)

ROOMLIB_SPAWN6(RoomFx_OrbitSetSpawn6)

ROOMLIB_REGISTER_TABLE_AT3(RoomFx_OrbitSetRegister, g_RoomOrbitSetDrawList)

ROOMLIB_START_AT3(RoomFx_OrbitSetStart, g_RoomOrbitSetUpdateList,
                  g_RoomOrbitSetInitList, g_RoomOrbitSetSpawnLayout,
                  RoomFx_OrbitSetClose)

ROOMLIB_CLOSE_TARGET(RoomFx_OrbitSetClose)

ROOMLIB_RETURN_ZERO(RoomFx_OrbitSetNop6)
