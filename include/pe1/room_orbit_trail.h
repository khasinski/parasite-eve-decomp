#ifndef PE1_ROOM_ORBIT_TRAIL_H
#define PE1_ROOM_ORBIT_TRAIL_H

#include "common.h"
#include "pe1/gte_types.h"

/* Orbit trail particle: a position, the orbit radius and the heading
 * angles the radius is projected along (a velocity while it falls). */
typedef struct RoomOrbitTrailParticle {
    s16 x, y, z;                  /* 0x00 */
    s16 radius;                   /* 0x06 */
    GteShortVector heading;       /* 0x08 */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomOrbitTrailParticle;

PE1_STATIC_ASSERT(sizeof(RoomOrbitTrailParticle) == 0x14,
                  room_orbit_trail_particle_size);

typedef struct RoomOrbitTrailFloor {
    s16 y;
} RoomOrbitTrailFloor;

/* Shared allocator context used by the contiguous orbit-trail burst family. */
typedef struct RoomOrbitTrailPoolChannel {
    s32 reserved[2];
    void *pool;
} RoomOrbitTrailPoolChannel;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOrbitTrailPoolChannel, pool) == 8,
                  room_orbit_trail_pool_offset);
PE1_STATIC_ASSERT(sizeof(RoomOrbitTrailPoolChannel) == 12,
                  room_orbit_trail_pool_channel_size);

/* Burst controller state: the actor anchor and two cleared counters. */
typedef struct RoomOrbitTrailBurst {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 counterA;                 /* 0x08 */
    s16 counterB;                 /* 0x0A */
} RoomOrbitTrailBurst;

/* Ring burst controller state (scene_e22): the burst anchor, two cleared
 * counters and the flag that enables the sweeping-trail volley. */
typedef struct RoomOrbitRingBurst {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 counterA;                 /* 0x08 */
    s16 counterB;                 /* 0x0A */
    s16 sweep;                    /* 0x0C */
} RoomOrbitRingBurst;

#endif
