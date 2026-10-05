#ifndef PE1_SCENE_E20_FLARE_H
#define PE1_SCENE_E20_FLARE_H

#include "pe1/gte_types.h"

typedef GteShortVector SceneE20Vec;

/* Shared by the trail controller that spawns particles and their callback.
 * Velocity doubles as the draw rotation for the spinning flare. */
typedef struct SceneE20Particle {
    /* 0x00 */ SceneE20Vec position;
    /* 0x08 */ SceneE20Vec velocity;
    /* 0x10 */ s16 kind;
    /* 0x12 */ s16 timer;
} SceneE20Particle;

/* Shared floor-height record used by particle motion and effect placement. */
typedef struct SceneE20Floor { s16 height; } SceneE20Floor;
extern SceneE20Floor D_800942EC;

int func_8018F028(int mode, SceneE20Particle *particle);

PE1_STATIC_ASSERT(sizeof(SceneE20Particle) == 0x14, scene_e20_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE20Particle, velocity) == 8,
                  scene_e20_particle_velocity_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE20Particle, kind) == 0x10,
                  scene_e20_particle_kind_offset);

#endif
