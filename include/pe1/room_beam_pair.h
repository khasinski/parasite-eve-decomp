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

/* Sprite parameter block drawn by func_800C42A4 for the drops. */
typedef struct RoomDropSprite {
    /* 0x00 */ u8 r, g, b;
    /* 0x03 */ u8 reserved03[7];
    /* 0x0A */ u16 depth;
} RoomDropSprite;

/* Falling drop field of the same rooms: sixteen drops, each sinking 0x96
 * per frame and released one at a time by the script. */
typedef struct RoomDropPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} RoomDropPoint;

typedef struct RoomDropField {
    /* 0x000 */ s8 live[0x20];
    /* 0x020 */ s16 depth[0x10];
    /* 0x040 */ s16 size[0x10];
    /* 0x060 */ RoomDropPoint drops[0x20];
    /* 0x160 */ GteMatrix matrix;
    /* 0x180 */ u16 yaw;
    /* 0x182 */ s16 first_step;
    /* 0x184 */ s16 released;
} RoomDropField;

typedef struct RoomDropSlot {
    /* 0x00 */ u8 state;
    /* 0x01 */ u8 command;
    /* 0x02 */ s16 step;
} RoomDropSlot;

int *func_800C2B28(int index);
RoomDropPoint *func_800C2B90(void *object, int kind, void *callback, void *data);
typedef struct RoomDropActor {
    /* 0x00 */ u8 pad00[0x44];
    /* 0x44 */ u8 shade;
    /* 0x45 */ u8 pad45[7];
    /* 0x4C */ s16 touched;
} RoomDropActor;

void *func_800C2B50(void);
void func_800C2EAC(int arg0);
void func_800C2FF0(int width, int height);
void func_800C3098(int arg0);
void func_800C42A4(RoomDropSprite *params, GteMatrix *matrix, int mode);
int func_800C6B90(void *position, int radius);
void *memset(void *dst, int value, unsigned int size);
void func_800C3238(int mode);
void func_800C4FC4(RoomBeamSprite *params, GteMatrix *matrix, int mode);

#endif
