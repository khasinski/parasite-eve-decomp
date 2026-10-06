/*
 * The three transform particles' module class: the plant, spawn,
 * registration, start and close methods the class table lists in front of
 * the particles (RoomLib_ThreeTransformParticles.c, which starts with the
 * class's closing no-op). They are the room module class's methods
 * (pe1/room_module.h) under the module's names, so that room_m393, m396
 * and m401 can link them in front of the falling sprite burst's class,
 * which uses the shared methods.
 *
 * room_m065, m085, m087, m391, m393, m394, m395, m396, m399, m401 and m402
 * link the class; the script and the lists are each room's own data
 * (pe1/room_three_particle_class.h).
 */
#include "room_lib.h"
#include "pe1/room_three_particle_class.h"

ROOMLIB_PLANT_TABLE(RoomLib_ThreeParticlePlantScript, g_RoomThreeParticleScript)

ROOMLIB_SPAWN6(RoomLib_ThreeParticleSpawn6)

ROOMLIB_REGISTER_TABLE(RoomLib_ThreeParticleRegister, g_RoomThreeParticleDrawList)

int RoomLib_ThreeParticleStart(void *o) {
    int result;

    if ((unsigned int)FieldEng_GetStatus(o) >= 2) {
        result = func_800C251C(o, g_RoomThreeParticleUpdateList);
        result |= func_800C2758(o, g_RoomThreeParticleInitList,
                                g_RoomThreeParticleSpawnLayout);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_ThreeParticleClose(o);
    }

    return 0;
}

ROOMLIB_CLOSE_TARGET(RoomLib_ThreeParticleClose)
