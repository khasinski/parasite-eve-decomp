#ifndef PE1_ROOM_M350_EFFECTS_H
#define PE1_ROOM_M350_EFFECTS_H

#include "pe1/gte_types.h"

typedef struct RoomM350ParticleMotion {
    union {
        GteShortVector rotation;
        struct {
            s16 speed;
            s16 yaw;
            s32 phase;
        } spawn;
    } value;
} RoomM350ParticleMotion;

typedef struct RoomM350ParticleTail {
    union {
        struct {
            s16 speed;
            s16 reserved12;
            s16 brightness;
            s16 frame;
            u8 hit;
            u8 reserved19[3];
        } active;
        struct {
            s16 size;
            s16 duration;
            s16 reserved14;
            s16 frame;
            u8 state;
            u8 reserved19[3];
        } spawn;
    } value;
} RoomM350ParticleTail;

typedef struct RoomM350Particle {
    GteShortVector position;
    RoomM350ParticleMotion motion;
    RoomM350ParticleTail tail;
} RoomM350Particle;

typedef struct RoomM350FlareParticle {
    s16 y;
    s16 z;
    s16 shade;
    s16 size;
} RoomM350FlareParticle;

typedef struct RoomM350CloudParticleState {
    union {
        struct {
            s16 animatedSize;
            s16 firstShade;
            s16 secondShade;
            s16 reserved12;
        } active;
        s32 spawnReserved[2];
    } value;
} RoomM350CloudParticleState;

typedef struct RoomM350CloudParticle {
    s32 *anchor;
    s16 position[3];
    s16 size;
    RoomM350CloudParticleState state;
} RoomM350CloudParticle;

typedef struct RoomM350SineParticle {
    GteShortVector position;
    s16 controls[4];
    s16 brightness;
    s16 size;
    s16 amplitude;
    s16 reserved16;
} RoomM350SineParticle;

typedef struct RoomM350Model {
    u8 reserved[2];
    u8 transformCount;
} RoomM350Model;

typedef struct RoomM350EffectOwner {
    u32 flags;
    u32 reserved04;
    void *asset;
    u8 reserved0C[0x40];
    u32 status;
} RoomM350EffectOwner;

typedef struct RoomM350EffectInstance {
    RoomM350EffectOwner *owner;
    u8 reserved04[10];
    u8 animation;
    u8 reserved0F[7];
    u16 frame;
    s16 reserved18;
    u16 previousFrame;
    u8 reserved1C[30];
    u16 yaw;
    u8 reserved3C[0x178];
    RoomM350Model *model;
    u8 reserved1B8[0x30];
    GteMatrix transform;
    u8 reserved208[0x30];
    GteMatrix *transforms;
} RoomM350EffectInstance;

typedef struct RoomM350EffectActor {
    s32 reserved[2];
    RoomM350EffectInstance *instance;
} RoomM350EffectActor;

typedef struct RoomM350EffectEmitter {
    s32 reserved[2];
    void *pool;
} RoomM350EffectEmitter;

PE1_STATIC_ASSERT(sizeof(RoomM350ParticleMotion) == 8,
                  room_m350_particle_motion_size);
PE1_STATIC_ASSERT(sizeof(RoomM350ParticleTail) == 12,
                  room_m350_particle_tail_size);
PE1_STATIC_ASSERT(sizeof(RoomM350Particle) == 28,
                  room_m350_particle_size);
PE1_STATIC_ASSERT(sizeof(RoomM350FlareParticle) == 8,
                  room_m350_flare_particle_size);
PE1_STATIC_ASSERT(sizeof(RoomM350CloudParticleState) == 8,
                  room_m350_cloud_particle_state_size);
PE1_STATIC_ASSERT(sizeof(RoomM350CloudParticle) == 20,
                  room_m350_cloud_particle_size);
PE1_STATIC_ASSERT(sizeof(RoomM350SineParticle) == 24,
                  room_m350_sine_particle_size);
PE1_STATIC_ASSERT(sizeof(RoomM350Model) == 3, room_m350_model_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350SineParticle, controls) == 8,
                  room_m350_sine_particle_controls_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350SineParticle, brightness) == 0x10,
                  room_m350_sine_particle_brightness_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Model, transformCount) == 2,
                  room_m350_model_transform_count_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350CloudParticle, position) == 4,
                  room_m350_cloud_particle_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350CloudParticle, size) == 0xA,
                  room_m350_cloud_particle_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350CloudParticle, state) == 0xC,
                  room_m350_cloud_particle_state_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Particle, motion) == 8,
                  room_m350_particle_motion_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Particle, tail) == 16,
                  room_m350_particle_tail_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectOwner, status) == 0x4C,
                  room_m350_owner_status_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectOwner, asset) == 8,
                  room_m350_owner_asset_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, animation) == 0xE,
                  room_m350_instance_animation_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, frame) == 0x16,
                  room_m350_instance_frame_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, yaw) == 0x3A,
                  room_m350_instance_yaw_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, model) == 0x1B4,
                  room_m350_instance_model_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, transform) == 0x1E8,
                  room_m350_instance_transform_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, transform.t) == 0x1FC,
                  room_m350_instance_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, transforms) == 0x238,
                  room_m350_instance_transforms_offset);

#endif
