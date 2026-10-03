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

/* Depth word closing each sprite parameter block, written as a record. */
typedef struct RoomBeamPairDepth {
    u16 value;
} RoomBeamPairDepth;

/* Falling drop field of the same rooms: sixteen drops, each sinking 0x96
 * per frame and released one at a time by the script. */
typedef struct RoomDropPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} RoomDropPoint;

typedef struct RoomDropField {
    /* 0x000 */ u8 live[0x60];
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
void *func_800C2B50(void);
void *memset(void *dst, int value, unsigned int size);
void func_800C3238(int mode);
void func_800C4FC4(void *params, GteMatrix *matrix, int mode);

#endif
