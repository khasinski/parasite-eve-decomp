#ifndef ROOM_M123_EFFECTS_H
#define ROOM_M123_EFFECTS_H

#include "common.h"

typedef struct RoomM123Particle {
    u16 frame;
    u16 offset;
    s16 scale;
    u16 reserved06;
} RoomM123Particle;

typedef struct RoomPulseParticle {
    u16 x, y, z, pad;
    s16 state, frame;
} RoomPulseParticle;

typedef struct RoomM123Wave {
    u16 x, y, z, pad;
    s16 state;
    u16 frame;
} RoomM123Wave;

typedef struct RoomM123Pool {
    u8 reserved_00[8];
    void *pool;
} RoomM123Pool;

extern RoomM123Pool *D_800F32D0, *D_800F33E0;

PE1_STATIC_ASSERT(sizeof(RoomM123Wave) == 0x0C,
                  room_m123_wave_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM123Wave, state) == 8,
                  room_m123_wave_state_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM123Wave, frame) == 0x0A,
                  room_m123_wave_frame_offset);
PE1_STATIC_ASSERT(sizeof(RoomM123Pool) == 0x0C,
                  room_m123_pool_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM123Pool, pool) == 8,
                  room_m123_pool_pointer_offset);

#endif
