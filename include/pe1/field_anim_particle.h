#ifndef PE1_FIELD_ANIM_PARTICLE_H
#define PE1_FIELD_ANIM_PARTICLE_H

#include "common.h"
#include "pe1/gte_types.h"

/* Moving glow particle shared by the four emitter setups and the bounce
 * updater. Positions and velocities wrap at sixteen bits; Y is tested as
 * signed for the bounce. Script records reserve 0x18 bytes per particle. */
typedef struct FieldAnimParticlePosition {
    u16 x, y, z, reserved06;
} FieldAnimParticlePosition;

typedef struct FieldAnimMovingParticle {
    u8 reserved00;
    u8 age;
    u8 lifetime;
    u8 reserved03;
    u16 brightness;
    s16 scale;
    FieldAnimParticlePosition position;
    GteShortVector velocity;
} FieldAnimMovingParticle;

PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimMovingParticle, position) == 8,
                  field_anim_moving_particle_position);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimMovingParticle, velocity) == 0x10,
                  field_anim_moving_particle_velocity);
PE1_STATIC_ASSERT(sizeof(FieldAnimMovingParticle) == 0x18,
                  field_anim_moving_particle_size);

/* Two independently transformed points rendered with the same glow scale. */
typedef struct FieldAnimTwinGlow {
    u8 reserved00[4];
    u16 brightness;
    s16 scale;
    FieldAnimParticlePosition position[2];
} FieldAnimTwinGlow;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTwinGlow, position) == 8,
                  field_anim_twin_glow_position);
PE1_STATIC_ASSERT(sizeof(FieldAnimTwinGlow) == 0x18,
                  field_anim_twin_glow_size);

extern FieldAnimParticlePosition D_800E2348, D_800E2350, D_800E2358, D_800E2360;

#endif
