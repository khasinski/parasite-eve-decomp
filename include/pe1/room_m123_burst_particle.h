#ifndef PE1_ROOM_M123_BURST_PARTICLE_H
#define PE1_ROOM_M123_BURST_PARTICLE_H

#include "common.h"

typedef struct RoomM123BurstParticle {
    u16 frame;                    /* 0x00 */
    u16 offset;                   /* 0x02 */
    s16 scale;                    /* 0x04 */
    u16 reserved06;               /* 0x06: eight-byte pool stride */
} RoomM123BurstParticle;

PE1_STATIC_ASSERT(sizeof(RoomM123BurstParticle) == 8,
                  room_m123_burst_particle_size);

#endif
