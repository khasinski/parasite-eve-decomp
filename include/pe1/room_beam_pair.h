#ifndef PE1_ROOM_BEAM_PAIR_H
#define PE1_ROOM_BEAM_PAIR_H

#include "common.h"
#include "pe1/gte_types.h"

/* Work area of the paired beam effect (rooms m174, m348, m383): the
 * anchor matrix copied from the script actor, a scale and a brightness. */
typedef struct RoomBeamPair {
    /* 0x00 */ GteMatrix matrix;
    /* 0x20 */ u8 pad20[0x10];
    /* 0x30 */ s16 scale;
    /* 0x32 */ u8 pad32[2];
    /* 0x34 */ u16 brightness;
} RoomBeamPair;

typedef struct RoomBeamPairActor {
    /* 0x000 */ u8 pad000[0x238];
    /* 0x238 */ u8 *model;
} RoomBeamPairActor;

/* Sprite parameter block drawn by func_800C4FC4 (one per beam half). */
typedef struct RoomBeamSprite {
    /* 0x00 */ void *texture;
    /* 0x04 */ u8 r0, g0, b0;
    /* 0x07 */ u8 reserved07;
    /* 0x08 */ u8 r1, g1, b1;
    /* 0x0B */ u8 reserved0B;
    /* 0x0C */ s16 offset;
    /* 0x0E */ s16 extent0;
    /* 0x10 */ s16 extent1;
    /* 0x12 */ s16 offset2;
    /* 0x14 */ u16 depth;
    /* 0x16 */ u8 reserved16[2];
} RoomBeamSprite;

PE1_STATIC_ASSERT(sizeof(RoomBeamSprite) == 0x18, room_beam_sprite_size);

void *func_800C2B50(void);
void *memset(void *dst, int value, unsigned int size);
void func_800C3238(int mode);
void func_800C4FC4(RoomBeamSprite *params, GteMatrix *matrix, int mode);

#endif
