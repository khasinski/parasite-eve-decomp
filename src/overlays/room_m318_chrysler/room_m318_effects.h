#ifndef ROOM_M318_EFFECTS_H
#define ROOM_M318_EFFECTS_H

#include "common.h"

typedef struct RoomM318Spark {
    u16 x;
    u16 y;
    u16 z;
    u16 speed;
} RoomM318Spark;

typedef struct RoomM318Emitter {
    u8 reserved_00[8];
    void *link;
} RoomM318Emitter;

extern RoomM318Emitter *D_800F33E0;

PE1_STATIC_ASSERT(sizeof(RoomM318Spark) == 8, room_m318_spark_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM318Emitter, link) == 8,
                  room_m318_emitter_link_offset);
PE1_STATIC_ASSERT(sizeof(RoomM318Emitter) == 0x0C,
                  room_m318_emitter_size);

#endif
