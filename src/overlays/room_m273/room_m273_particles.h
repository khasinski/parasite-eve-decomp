#ifndef ROOM_M273_PARTICLES_H
#define ROOM_M273_PARTICLES_H

#include "common.h"

/* Shared 8-byte payload used by the room_m273 rising-sprite callbacks. */
typedef struct RoomM273RisingParticle {
    s16 x;
    u16 y;
    s16 z;
    u16 speed;
} RoomM273RisingParticle;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingParticle, y) == 2,
                  room_m273_rising_particle_y_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingParticle, z) == 4,
                  room_m273_rising_particle_z_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingParticle, speed) == 6,
                  room_m273_rising_particle_speed_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273RisingParticle) == 8,
                  room_m273_rising_particle_size);

#endif
