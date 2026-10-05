#ifndef ROOM_M273_ARC_H
#define ROOM_M273_ARC_H

#include "common.h"

/* Eight-byte sprite-pool payload shared by the room arc emitter and callback. */
typedef struct RoomM273ArcCallbackState {
    u16 unused0;
    u16 value;
    u16 unused4;
    u16 step;
} RoomM273ArcCallbackState;

PE1_STATIC_ASSERT(sizeof(RoomM273ArcCallbackState) == 8,
                  room_m273_arc_callback_state_size);

#endif
