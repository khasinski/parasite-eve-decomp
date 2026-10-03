#ifndef PE1_ROOM_DRIFT_PULSE_H
#define PE1_ROOM_DRIFT_PULSE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Two-phase room effect: phase 0 drifts a sprite and draws a glow strip at a
 * fixed anchor, phase 1 draws a swelling sprite at the drifted position. */
typedef struct RoomDriftPulseState {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
    /* 0x06 */ s16 phase;
    /* 0x08 */ u16 vx;
    /* 0x0A */ u16 vy;
    /* 0x0C */ u16 vz;
} RoomDriftPulseState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDriftPulseState, vz) == 0x0C,
                  room_drift_pulse_vz_offset);

extern GteShortVector D_80190758;
int func_80077AA4(int, int);
int func_80077DC4(int angle);

#endif
