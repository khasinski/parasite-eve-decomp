#ifndef PE1_ROOM_MOTION_INIT_H
#define PE1_ROOM_MOTION_INIT_H

#include "common.h"
#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"

/* Motion particle setup shared by rooms m075, m080 and m082: copies three
 * frames from the owner's model, then seeds the five overlay glyph records
 * and the two ten-entry particle tables. */

typedef struct RoomMotionInitModel {
    RoomSpriteMatrix base;          /* 0x000 */
    u8 pad020[0x100];
    RoomSpriteMatrix primary;       /* 0x120 */
    u8 pad140[0x60];
    RoomSpriteMatrix secondary;     /* 0x1A0 */
} RoomMotionInitModel;

typedef struct RoomMotionInitLink {
    u8 pad000[0x238];
    RoomMotionInitModel *model;     /* 0x238 */
} RoomMotionInitLink;

typedef struct RoomMotionInitOwner {
    u8 pad00[8];
    RoomMotionInitLink *link;       /* 0x08 */
} RoomMotionInitOwner;

typedef struct RoomMotionInitState {
    RoomMotionInitLink *link;       /* 0x00 */
    RoomSpriteMatrix base;          /* 0x04 */
    RoomSpriteMatrix secondary;     /* 0x24 */
    RoomSpriteMatrix primary;       /* 0x44 */
    s16 timer;                      /* 0x64 */
    s16 field66;
    s16 field68;
    u8 pad6A[2];
    void *allocation;               /* 0x6C */
} RoomMotionInitState;

/* 16-byte glyph record. */
typedef struct RoomMotionGlyph {
    u8 red;
    u8 green;
    u8 blue;
    u8 pad03;
    u8 size;
    u8 visible;
    u8 flags;
    u8 pad07;
    s16 x;
    s16 y;
    u8 pad0C[4];
} RoomMotionGlyph;

/* 0x44-byte particle record. */
typedef struct RoomMotionParticle {
    u8 pad00;
    u8 mode;
    u8 pad02[2];
    s16 alpha;
    s16 kind;
    s16 life;
    s16 speed;
    s16 drift;
    u8 pad0E[2];
    u8 red;
    u8 green;
    u8 blue;
    u8 pad13[0x31];
} RoomMotionParticle;

extern RoomMotionGlyph D_801940B8;
extern RoomMotionParticle D_801940C8[10];
extern RoomMotionParticle D_80194370[10];
extern RoomMotionGlyph D_80194618[4];

void *func_8006DC18(int type);

#endif /* PE1_ROOM_MOTION_INIT_H */
