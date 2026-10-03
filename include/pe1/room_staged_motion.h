#ifndef PE1_ROOM_STAGED_MOTION_H
#define PE1_ROOM_STAGED_MOTION_H

#include "common.h"

/* Staged motion effect shared by room_m174 and room_m383: up to eight
 * 16.16 fixed-point records orbiting an origin, each stepping through a
 * sprite stage (kind 1), a flat sprite plus model stage (kind 2..0x14) and
 * off (kind 0). Positions are 16.16; readers take the integer half. */
typedef struct RoomStagedMotionRecord {
    s32 x;
    s32 y;
    s32 z;
    s32 pad0C;
} RoomStagedMotionRecord;

typedef struct RoomStagedMotionState {
    s16 x;                            /* 0x00 */
    s16 y;                            /* 0x02 */
    s16 z;                            /* 0x04 */
    s16 pad06;
    RoomStagedMotionRecord record[8]; /* 0x08 */
    u16 phase[8];                     /* 0x88 */
    s16 angle[8];                     /* 0x98 */
    s32 radius[8];                    /* 0xA8 */
    u8 kind[8];                       /* 0xC8 */
    u16 depth[8];                     /* 0xD0 */
    s32 fallStep;                     /* 0xE0 */
    s32 radiusStep;                   /* 0xE4 */
    s16 count;                        /* 0xE8 */
    u16 size;                         /* 0xEA */
    s16 remaining;                    /* 0xEC */
} RoomStagedMotionState;

PE1_STATIC_ASSERT(sizeof(RoomStagedMotionRecord) == 0x10,
                  room_staged_motion_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomStagedMotionState, phase) == 0x88,
                  room_staged_motion_phase_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomStagedMotionState, kind) == 0xC8,
                  room_staged_motion_kind_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomStagedMotionState, remaining) == 0xEC,
                  room_staged_motion_remaining_offset);

#endif
