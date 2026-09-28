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

#endif
