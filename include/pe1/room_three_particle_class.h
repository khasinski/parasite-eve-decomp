#ifndef PE1_ROOM_THREE_PARTICLE_CLASS_H
#define PE1_ROOM_THREE_PARTICLE_CLASS_H

#include "pe1/room_module.h"

/* The three transform particles' module class
 * (src/overlays/room_lib/RoomLib_ThreeParticleClass.c): the plant, spawn,
 * registration, start and close methods of the room module class, in
 * class-table slot order, and the script and lists they hand the field
 * engine, named in each room's symbol file. The class's no-ops belong to
 * RoomLib_ThreeTransformParticles.c. */
extern int g_RoomThreeParticleScript[];
extern RoomModuleHandler g_RoomThreeParticleDrawList[];
extern RoomModuleHandler g_RoomThreeParticleUpdateList[];
extern RoomModuleHandler g_RoomThreeParticleInitList[];
extern int g_RoomThreeParticleSpawnLayout[];

int RoomLib_ThreeParticlePlantScript(void);
int RoomLib_ThreeParticleSpawn6(int a, int b, int c, int d, int e, int f);
int RoomLib_ThreeParticleRegister(void *o);
int RoomLib_ThreeParticleStart(void *o);
int RoomLib_ThreeParticleClose(struct RoomEnt *o);

#endif
