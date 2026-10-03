#ifndef PE1_ROOM_BOUNCE_GLINT_H
#define PE1_ROOM_BOUNCE_GLINT_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Glint particle spawned by the room_m086 controller: phase 0 drifts for 24
 * frames, phase 1 falls and bounces on the floor height for 32 frames. */
typedef struct RoomBounceGlint {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
    /* 0x06 */ s16 flag;
    /* 0x08 */ s16 vx;
    /* 0x0A */ s16 vy;
    /* 0x0C */ s16 vz;
    /* 0x0E */ s16 pad0E;
    /* 0x10 */ s16 timer;
    /* 0x12 */ s16 phase;
} RoomBounceGlint;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomBounceGlint, phase) == 0x12,
                  room_bounce_glint_phase_offset);

/* Floor height, read as a one-field record so the bounce test stays below
 * the velocity store like retail. */
typedef struct RoomBounceGlintFloor {
    s16 height;
} RoomBounceGlintFloor;

extern RoomBounceGlintFloor D_800942EC;
extern u16 D_800E11EA;
int func_80077AA4(int, int);
int func_80077DC4(int angle);

#endif
