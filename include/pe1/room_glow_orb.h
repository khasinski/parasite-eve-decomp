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

/* Sprite parameter block handed to func_800C42A4 with each glow layer. */
typedef struct RoomGlowOrbSprite {
    /* 0x00 */ u8 r;
    /* 0x01 */ u8 g;
    /* 0x02 */ u8 b;
    /* 0x03 */ u8 pad03[7];
    /* 0x0A */ u16 depth;
} RoomGlowOrbSprite;

typedef struct RoomGlowOrbActorState {
    /* 0x00 */ u8 pad00[0x4C];
    /* 0x4C */ int flags;
} RoomGlowOrbActorState;

/* 16.16 world coordinate; the integer half is read as a bitfield. */
typedef union RoomGlowOrbCoord {
    s32 word;
    struct {
        int fraction : 16;
        int integer : 16;
    } part;
} RoomGlowOrbCoord;

typedef struct RoomGlowOrbActor {
    /* 0x00 */ RoomGlowOrbActorState *state;
    /* 0x04 */ u8 pad04[0x24];
    /* 0x28 */ RoomGlowOrbCoord x;
    /* 0x2C */ RoomGlowOrbCoord y;
    /* 0x30 */ RoomGlowOrbCoord z;
} RoomGlowOrbActor;

extern RoomGlowOrbActor *D_8009D254;

void *memset(void *dst, int value, unsigned int size);
void func_800C2EAC(int arg0);
void func_800C2FF0(int width, int height);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(RoomGlowOrbSprite *sprite, GteMatrix *matrix, int mode);
int func_800C61A8(GteShortVector *point, GteMatrix *matrix);
typedef struct RoomGlowOrbObject {
    /* 0x00 */ u8 pad00[8];
    /* 0x08 */ u32 **link;
} RoomGlowOrbObject;

int FieldEng_GetStatus(RoomGlowOrbObject *object);

int *func_800C2B10(int index);
int *func_800C2B28(int index);

#endif
