#ifndef PE1_ROOM_SWIRL_RISE_H
#define PE1_ROOM_SWIRL_RISE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Room effect that spirals a sprite up from an anchor (phase 0) and then
 * slides it away while a wide glow strip pulses at the anchor (phase 1). */
typedef struct RoomSwirlRiseState {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
    /* 0x06 */ s16 pad06;
    /* 0x08 */ s16 phase;
    /* 0x0A */ s16 angle;
    /* 0x0C */ s16 radius;
} RoomSwirlRiseState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSwirlRiseState, radius) == 0x0C,
                  room_swirl_rise_radius_offset);

extern RenderColor D_8018F1D8;
int func_80077AA4(int, int);
int func_80077CF4(int angle);
int func_80077DC4(int angle);

#endif
