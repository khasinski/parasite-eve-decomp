#ifndef PE1_ROOM_M089_TRAIL_H
#define PE1_ROOM_M089_TRAIL_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* room_m089 spiral trail: a sprite that spirals in towards an anchor while
 * five older positions follow it as fading line segments, then flashes. */
typedef struct RoomM089SpiralTrail {
    /* 0x00 */ GteShortVector points[6]; /* pad: 0 radius, 1 angle step */
    /* 0x30 */ s16 timer;
    /* 0x32 */ s16 state;
    /* 0x34 */ s16 angle;
    /* 0x36 */ s16 rise;
} RoomM089SpiralTrail;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM089SpiralTrail, rise) == 0x36,
                  room_m089_spiral_trail_rise_offset);

extern RenderColor D_8018F1CC;
extern RenderColor D_8018F1D0;
int func_80077AA4(int, int);
int func_80077CF4(int angle);
int func_80077DC4(int angle);
void func_800D2B58(void *, void *, void *, void *, int, int, int);

#endif
