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

#endif
