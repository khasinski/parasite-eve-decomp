/*
 * The ground eruption's module class: the seven methods the class table
 * lists in front of the eruption (RoomEffect_GroundEruption.c). The
 * registration and the start method act only while the field engine runs
 * the object (status 3), and the close method clears the target's owner
 * bits without marking the target closed (compare RoomLib_CloseTarget).
 *
 * room_m141, m146, m153, m154, m328 and scene_e02, e04 and e05 link the
 * class; in room_m146 it comes before the falling sprite burst's module,
 * which uses the shared class methods. The script and the lists are each
 * overlay's own data (pe1/room_ground_eruption.h).
 */
#include "room_lib.h"
#include "pe1/room_ground_eruption.h"

ROOMLIB_RETURN_ZERO(RoomEffect_GroundEruptionNop0)

ROOMLIB_PLANT_TABLE(RoomEffect_GroundEruptionPlantScript, g_RoomEruptionScript)

ROOMLIB_SPAWN6(RoomEffect_GroundEruptionSpawn6)

ROOMLIB_REGISTER_TABLE_AT3(RoomEffect_GroundEruptionRegister, g_RoomEruptionDrawList)

ROOMLIB_START_AT3(RoomEffect_GroundEruptionStart, g_RoomEruptionUpdateList,
                  g_RoomEruptionInitList, g_RoomEruptionSpawnLayout,
                  RoomEffect_GroundEruptionRelease)

int RoomEffect_GroundEruptionRelease(RoomEnt *o) {
    o->state = 4;
    if ((unsigned int)FieldEng_GetStatus(o) >= 2) {
        RoomRenderNode *target = o->link->target;
        target->flags &= 0xC0FFFFFF;
    }
    return 0;
}

ROOMLIB_RETURN_ZERO(RoomEffect_GroundEruptionNop6)
