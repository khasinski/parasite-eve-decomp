#ifndef PE1_ROOM_DRIFT_PULSE_H
#define PE1_ROOM_DRIFT_PULSE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_m023_effects.h"

/* Two-phase room effect: phase 0 drifts a sprite and draws a glow strip at a
 * fixed anchor, phase 1 draws a swelling sprite at the drifted position. */
typedef RoomM023DriftPulseCallbackView RoomDriftPulseState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDriftPulseState, vz) == 0x0C,
                  room_drift_pulse_vz_offset);

extern GteShortVector D_80190758;
int func_80077AA4(int, int);
int func_80077DC4(int angle);

#endif
