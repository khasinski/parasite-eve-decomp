#ifndef PE1_ROOM_PARTICLE_STATE_H
#define PE1_ROOM_PARTICLE_STATE_H

#include "common.h"

/* Shared motion and lifetime fields for room particle effect records. */
typedef struct RoomParticleState {
    short x;
    short y;
    short z;
    short size;
    short vx;
    short vy;
    short vz;
    short angle;
    short state;
    short timer;
} RoomParticleState;

PE1_STATIC_ASSERT(sizeof(RoomParticleState) == 0x14,
                  room_particle_state_size);

#endif /* PE1_ROOM_PARTICLE_STATE_H */
