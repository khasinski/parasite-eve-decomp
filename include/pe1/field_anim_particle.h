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

/* Two position/displacement slots updated for eight frames. */
typedef struct FieldAnimTwoPointMotion {
    u8 reserved00[3];
    u8 age;
    u8 reserved04[12];
    FieldAnimParticlePosition position[2];
    FieldAnimParticlePosition velocity[2];
} FieldAnimTwoPointMotion;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTwoPointMotion, position) == 0x10,
                  field_anim_two_point_position);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTwoPointMotion, velocity) == 0x20,
                  field_anim_two_point_velocity);
PE1_STATIC_ASSERT(sizeof(FieldAnimTwoPointMotion) == 0x30,
                  field_anim_two_point_motion_size);

/* Spark setup, draw and fade callbacks share this 0x10-byte prefix.
 * Position and displacement arithmetic wraps at sixteen bits. */
typedef struct FieldAnimSparkPoint {
    u8 reserved00;
    u8 brightness;
    u8 reserved02;
    u8 scaleStep;
    struct { u16 x, y, z; } position;
    struct { u16 x, y, z; } displacement;
} FieldAnimSparkPoint;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimSparkPoint, position) == 4,
                  field_anim_spark_position);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimSparkPoint, displacement) == 0xA,
                  field_anim_spark_displacement);
PE1_STATIC_ASSERT(sizeof(FieldAnimSparkPoint) == 0x10,
                  field_anim_spark_point_size);

/* Eight sparks stored by coordinate, followed by matching displacements. */
typedef struct FieldAnimSparkCloud {
    FieldAnimSparkPoint center;
    u16 position[3][8];
    u16 velocity[3][8];
} FieldAnimSparkCloud;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimSparkCloud, position) == 0x10,
                  field_anim_spark_cloud_position);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimSparkCloud, velocity) == 0x40,
                  field_anim_spark_cloud_velocity);
PE1_STATIC_ASSERT(sizeof(FieldAnimSparkCloud) == 0x70,
                  field_anim_spark_cloud_size);

/* Six-frame spark: randomized position, fixed brightness and age-driven draw. */
typedef struct FieldAnimTimedSpark {
    u8 reserved00;
    u8 brightness;
    u8 reserved02;
    u8 age;
    u16 reserved04;
    struct { u16 x, y, z; } position;
} FieldAnimTimedSpark;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTimedSpark, position) == 6,
                  field_anim_timed_spark_position);
PE1_STATIC_ASSERT(sizeof(FieldAnimTimedSpark) == 0xC,
                  field_anim_timed_spark_size);

/* Single glow point used by transformed and fixed-position emitters. */
typedef struct FieldAnimGlowPoint {
    u8 reserved00[4];
    u16 brightness;
    s16 scale;
    FieldAnimParticlePosition position;
} FieldAnimGlowPoint;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimGlowPoint, position) == 8,
                  field_anim_glow_point_position);
PE1_STATIC_ASSERT(sizeof(FieldAnimGlowPoint) == 0x10,
                  field_anim_glow_point_size);

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

/* Glow point with an emitter matrix copied at setup and rescaled at draw. */
typedef struct FieldAnimMatrixGlow {
    u8 reserved00[4];
    u16 brightness;
    s16 scale;
    FieldAnimParticlePosition position;
    GteMatrixStorage transform;
} FieldAnimMatrixGlow;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimMatrixGlow, transform) == 0x10,
                  field_anim_matrix_glow_transform);
PE1_STATIC_ASSERT(sizeof(FieldAnimMatrixGlow) == 0x30,
                  field_anim_matrix_glow_size);

extern FieldAnimParticlePosition D_800E27F0, D_800E2808;

extern FieldAnimParticlePosition D_800E2348, D_800E2350, D_800E2358, D_800E2360;

#endif
