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

typedef struct RoomM350EffectOwner {
    u32 flags;
    u8 reserved04[72];
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
    u8 reserved3C[0x1C0];
    s32 position[3];
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
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Particle, motion) == 8,
                  room_m350_particle_motion_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Particle, tail) == 16,
                  room_m350_particle_tail_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectOwner, status) == 0x4C,
                  room_m350_owner_status_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, animation) == 0xE,
                  room_m350_instance_animation_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, frame) == 0x16,
                  room_m350_instance_frame_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, yaw) == 0x3A,
                  room_m350_instance_yaw_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350EffectInstance, position) == 0x1FC,
                  room_m350_instance_position_offset);

#endif
