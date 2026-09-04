#ifndef PE1_ROOM_FX_H
#define PE1_ROOM_FX_H

#include "common.h"

/* Common room-overlay sprite effect data used by m137-family rooms. */
typedef struct RoomSpriteMatrix {
    short m[3][3];
    short pad;
    int t[3];
} RoomSpriteMatrix;

typedef struct RoomFxSeed8 {
    unsigned char bytes[8];
} RoomFxSeed8;

typedef struct RoomFxVec4 {
    int x;
    int y;
    int z;
    int w;
} RoomFxVec4;

typedef struct RoomUniformSpriteFxParams {
    short x;
    short y;
    short z;
    short pad6;
    short scale;
    unsigned short depth;
} RoomUniformSpriteFxParams;

typedef struct RoomOrbitParticlePosition {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned short pad6;
} RoomOrbitParticlePosition;

typedef struct RoomOrbitParticleVelocity {
    short x;
    unsigned short angle;
    short z;
    short pad6;
} RoomOrbitParticleVelocity;

/* View of one position together with its velocity in the parallel array. */
typedef struct RoomOrbitParticleLaneView {
    RoomOrbitParticlePosition position;
    unsigned char remainingPositions[7 * sizeof(RoomOrbitParticlePosition)];
    RoomOrbitParticleVelocity velocity;
} RoomOrbitParticleLaneView;

typedef struct RoomOrbitParticleState {
    RoomOrbitParticlePosition position[8];
    RoomOrbitParticleVelocity velocity[8];
    unsigned short height;
    short decay;
    unsigned char frame;
    unsigned char intensity;
    short radius;
    short radiusStep;
} RoomOrbitParticleState;

typedef struct RoomOrbitBurstVector {
    short x;
    short y;
    short z;
    short pad6;
} RoomOrbitBurstVector;

typedef struct RoomOrbitBurstState {
    RoomOrbitBurstVector position[8];
    RoomOrbitBurstVector velocity[8];
    RoomOrbitBurstVector secondary[8];
    unsigned char active[8];
    unsigned char frame[8];
    short scale;
    unsigned short depth;
    unsigned char count;
    unsigned char padD5;
    short phaseStep;
} RoomOrbitBurstState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOrbitBurstState, velocity) == 0x40,
                  room_orbit_burst_velocity_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOrbitBurstState, secondary) == 0x80,
                  room_orbit_burst_secondary_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOrbitBurstState, active) == 0xC0,
                  room_orbit_burst_active_offset);
PE1_STATIC_ASSERT(sizeof(RoomOrbitBurstState) == 0xD8,
                  room_orbit_burst_state_size);

typedef struct RoomSpriteFxParams {
    short x;
    short y;
    short z;
    unsigned char pad6[0xA];
    short scale;
    unsigned short depth;
    unsigned char pad14;
    unsigned char alpha;
} RoomSpriteFxParams;

typedef struct RoomOrbitSpriteFxParams {
    short x;
    short y;
    short z;
    unsigned char pad6[0xA];
    short scale;
    unsigned short depth;
    unsigned char pad14[2];
    unsigned short alpha;
} RoomOrbitSpriteFxParams;

typedef struct RoomSeededSpriteFxParams {
    short x;
    short y;
    short z;
    unsigned char pad6[0x2];
    RoomFxSeed8 seed;
    short scale;
    unsigned short depth;
} RoomSeededSpriteFxParams;

/* Per-particle state for the room sparkle/drift effect. */
typedef struct RoomFxDriftParticle {
    short x;
    short y;
    short z;
    short pad6;
    int radius;
} RoomFxDriftParticle;

typedef struct RoomFxTrajectoryParticle {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned short pad6;
} RoomFxTrajectoryParticle;

typedef struct RoomFxGroundSpriteParams {
    short x;
    short pad02;
    short z;
    unsigned char pad06[0x7C];
    unsigned short depth;
} RoomFxGroundSpriteParams;

typedef struct RoomFxTimedRenderState {
    unsigned char pad00[8];
    short disabled;
    unsigned short frame;
} RoomFxTimedRenderState;

typedef struct RoomFallingParticleControl {
    unsigned char pad00;
    unsigned char state;
} RoomFallingParticleControl;

typedef struct RoomFallingParticleState {
    unsigned char active;
    unsigned char pad01;
    unsigned char frame;
    unsigned char pad03;
    unsigned short phase;
    short intensity;
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned char pad0E[0xA];
    unsigned short velocityX;
    unsigned short velocityY;
    unsigned short velocityZ;
} RoomFallingParticleState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFallingParticleState, velocityX) == 0x18,
                  room_falling_particle_velocity_x_offset);
PE1_STATIC_ASSERT(sizeof(RoomFallingParticleState) == 0x1E,
                  room_falling_particle_state_size);

typedef struct RoomEightParticleVector {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned short pad06;
} RoomEightParticleVector;

typedef struct RoomEightParticleState {
    int activeCount;
    RoomEightParticleVector position[8];
    RoomFxSeed8 velocity[8];
    unsigned short scale[8];
    short fade[8];
    unsigned short angle[8];
    unsigned char active[8];
} RoomEightParticleState;

typedef struct RoomEightParticleControl {
    unsigned char pad00;
    unsigned char state;
    short frame;
} RoomEightParticleControl;

typedef struct RoomEightParticleContext {
    char *root;
    unsigned char pad04[0x34];
    int baseX;
    int baseY;
    int baseZ;
} RoomEightParticleContext;

typedef struct RoomFxTransform {
    int pad00[5];
    int x;
    int y;
    int z;
} RoomFxTransform;

typedef struct RoomFxTransformOwner {
    unsigned char pad00[0x238];
    RoomFxTransform *transforms;
} RoomFxTransformOwner;

typedef struct RoomFxControl {
    unsigned char pad00[2];
    short frame;
} RoomFxControl;

/* State shared by the paired rising-sprite effects in the m049 family. */
typedef struct RoomFxPairedSpriteState {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned char pad06[2];
    short velocityX;
    short velocityY;
    short velocityZ;
    unsigned char pad0E[2];
    short alpha;
    unsigned short alphaStep;
    unsigned short sparkleTimer;
    unsigned char pad16[2];
    unsigned short sparkleX;
    unsigned short sparkleY;
    unsigned short sparkleZ;
    unsigned char pad1E[2];
    unsigned short sparkleAlpha;
    short sparkleLife;
    short resourceSelector;
    short active;
    short height;
    short width;
    short phase;
    short transformIndex;
    unsigned int counter;
    short minimumHeight;
} RoomFxPairedSpriteState;

PE1_STATIC_ASSERT(sizeof(RoomFxTransform) == 0x20,
                  room_fx_transform_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxTransformOwner, transforms) == 0x238,
                  room_fx_transform_owner_transforms_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxPairedSpriteState, sparkleX) == 0x18,
                  room_fx_paired_sprite_sparkle_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxPairedSpriteState, resourceSelector) == 0x24,
                  room_fx_paired_sprite_resource_selector_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxPairedSpriteState, transformIndex) == 0x2E,
                  room_fx_paired_sprite_transform_index_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxPairedSpriteState, minimumHeight) == 0x34,
                  room_fx_paired_sprite_minimum_height_offset);
PE1_STATIC_ASSERT(sizeof(RoomFxPairedSpriteState) == 0x38,
                  room_fx_paired_sprite_state_size);

PE1_STATIC_ASSERT(sizeof(RoomEightParticleVector) == 8,
                  room_eight_particle_vector_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEightParticleState, velocity) == 0x44,
                  room_eight_particle_velocity_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEightParticleState, scale) == 0x84,
                  room_eight_particle_scale_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEightParticleState, fade) == 0x94,
                  room_eight_particle_fade_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEightParticleState, angle) == 0xA4,
                  room_eight_particle_angle_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEightParticleState, active) == 0xB4,
                  room_eight_particle_active_offset);
PE1_STATIC_ASSERT(sizeof(RoomEightParticleState) == 0xBC,
                  room_eight_particle_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEightParticleContext, baseX) == 0x38,
                  room_eight_particle_context_base_x_offset);

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxGroundSpriteParams, depth) == 0x82,
                  room_fx_ground_sprite_depth_offset);

typedef struct RoomFxDriftState {
    unsigned short x;
    short pad02;
    unsigned short z;
    short pad06;
    short dx;
    short pad0A;
    short dz;
    short pad0E;
    unsigned short phase10;
    short limit12;
    short phase14;
    unsigned short counter16;
} RoomFxDriftState;

/* Parameters consumed by func_800C4E50 for the paired room/scene effect. */
typedef struct RoomFxEmitterParams {
    void *source;
    unsigned char color0[3];
    unsigned char pad07;
    unsigned char color1[3];
    unsigned char pad0B;
    short mode;
    short extent0;
    short extent1;
    short offset;
    short intensity;
    short pad16;
} RoomFxEmitterParams;

typedef struct RoomFxPairedEmitterState {
    unsigned char header[8];
    unsigned char sourceData[0x100];
    RoomFxEmitterParams primary;
    RoomFxEmitterParams secondary;
    short timer;
    short intensity;
    short phase;
} RoomFxPairedEmitterState;

PE1_STATIC_ASSERT(sizeof(RoomFxEmitterParams) == 0x18,
                  room_fx_emitter_params_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxPairedEmitterState, primary) == 0x108,
                  room_fx_paired_primary_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxPairedEmitterState, secondary) == 0x120,
                  room_fx_paired_secondary_offset);
PE1_STATIC_ASSERT(sizeof(RoomFxPairedEmitterState) == 0x140,
                  room_fx_paired_state_size);

#endif
