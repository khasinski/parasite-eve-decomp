#ifndef PE1_ROOM_FLOOR_H
#define PE1_ROOM_FLOOR_H

#include "common.h"

/* 0x800942EC: the floor height (render-space Y) of the current room. Field
 * and room effects bounce on or drop to it, battle sets it through context
 * field 255 and the frame loop clears it. Most readers load it signed (lh),
 * a few compare it as an unsigned halfword (lhu). Reads are union-member
 * reads, which GCC orders after stores through particle and emitter records
 * as retail does. The incomplete array keeps the retail absolute address in
 * -G8 units such as the frame loop. */
typedef union RoomFloorY {
    s16 y;
    u16 raw;
} RoomFloorY;

extern RoomFloorY g_RoomFloorY[];

#endif
