#ifndef ROOM_M273_SWAY_H
#define ROOM_M273_SWAY_H

#include "common.h"
#include "pe1/gte_types.h"

/* Two transformed boss points and their animation-15 sway state at 0x8019AF74. */
typedef struct RoomM273SwayState {
    GteShortVector points[2];
    GteShortVector hits[2];
    s16 animation;
    s16 frame;
    s16 frame_1A;
    s16 repeat;
    s16 sway_timer;
    s16 sway_step;
    s16 cooldown;
    u8 done;
} RoomM273SwayState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273SwayState, points) == 0,
                  room_m273_sway_points_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273SwayState, animation) == 0x20,
                  room_m273_sway_animation_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273SwayState, frame) == 0x22,
                  room_m273_sway_frame_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273SwayState, sway_timer) == 0x28,
                  room_m273_sway_timer_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273SwayState, done) == 0x2E,
                  room_m273_sway_done_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273SwayState) == 0x30,
                  room_m273_sway_state_size);

extern RoomM273SwayState D_8019AF74;

#endif
