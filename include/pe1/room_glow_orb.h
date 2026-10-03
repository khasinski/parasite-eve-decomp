#ifndef PE1_ROOM_GLOW_ORB_H
#define PE1_ROOM_GLOW_ORB_H

#include "common.h"
#include "pe1/gte_types.h"

/* Glow orb effect state (rooms m034, m174, m383): a camera-facing glow
 * placed by a fixed axis matrix, turned with the script's yaw and drawn
 * as a stack of scaled sprites. */
typedef struct RoomGlowOrb {
    /* 0x00 */ s8 state;
    /* 0x01 */ u8 slot;
    /* 0x02 */ u8 flag2;
    /* 0x03 */ u8 pad03;
    /* 0x04 */ s16 size;
    /* 0x06 */ u16 depth;
    /* 0x08 */ s16 x;
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 z;
    /* 0x0E */ u8 pad0E[2];
    /* 0x10 */ s16 h10;
    /* 0x12 */ s16 h12;
    /* 0x14 */ s16 h14;
    /* 0x16 */ u8 pad16[2];
    /* 0x18 */ GteShortVector offset;
    /* 0x20 */ u8 pad20[4];
    /* 0x24 */ GteMatrix matrix;
} RoomGlowOrb;

/* Room scalars read as one-field records so their loads stay below the
 * preceding state stores, as retail orders them. */
typedef struct RoomGlowOrbWord {
    int value;
} RoomGlowOrbWord;

int *func_800C2B10(int index);
int *func_800C2B28(int index);

#endif
