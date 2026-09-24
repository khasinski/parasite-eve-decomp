#ifndef ROOM_LIB_DRAW_MOTION_H
#define ROOM_LIB_DRAW_MOTION_H

#include "room_lib.h"

typedef struct RoomMotionContext {
    RoomLink *link;
    u8 pad04[0x68];
    u8 owner;
} RoomMotionContext;

typedef struct RoomMotionTransform {
    u8 pad00[0x120];
    RoomSpriteMatrix primary;
    u8 pad140[0x60];
    RoomSpriteMatrix secondary;
} RoomMotionTransform;

typedef struct RoomMotionLookup {
    s16 id;
    s16 pad;
} RoomMotionLookup;

typedef struct RoomMotionState {
    RoomSpriteMatrix matrix[2];
    u8 pad40[0x120];
    RoomMotionLookup lookup[8];
    u16 depth;
    s16 count;
} RoomMotionState;

typedef struct RoomMotionParticle {
    s16 primaryX;
    s16 primaryY;
    s16 primaryZ;
    s16 pad46;
    s16 secondaryX;
    s16 secondaryY;
    s16 secondaryZ;
    s16 pad4E;
} RoomMotionParticle;

typedef struct RoomMotionCursor {
    u8 pad00[0x40];
    RoomMotionParticle particle;
} RoomMotionCursor;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionContext, owner) == 0x6C,
                  room_motion_context_owner_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTransform, secondary) == 0x1A0,
                  room_motion_secondary_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionState, depth) == 0x180,
                  room_motion_depth_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionCursor, particle.secondaryZ) == 0x4C,
                  room_motion_cursor_secondary_z_offset);

extern RoomFxVec4 D_8018EFFC;
extern s16 D_801940C0;
extern s16 D_801940C2;
extern u8 D_80194008[];

#endif
