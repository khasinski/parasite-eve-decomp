#ifndef PE1_ROOM_WINDOW_H
#define PE1_ROOM_WINDOW_H

#include "common.h"
#include "pe1/gte_types.h"

/* The window effect: a script object that flies a camera-facing frame from
 * its anchor actor towards a target actor, spinning it around the flight
 * axis, until it reaches the target or arrives near the player. */

/* 16.16 fixed-point coordinate with its integer half addressable. */
typedef union RoomWindowFixed {
    s32 value;
    struct {
        u16 fraction;
        s16 integer;
    } part;
} RoomWindowFixed;

/* One packed entry of the PSY-Q sine table: sine in the low half, cosine
 * in the high half. */
typedef union RoomWindowTrig {
    s32 word;
    struct {
        s16 sin;
        s16 cos;
    } part;
} RoomWindowTrig;

typedef struct RoomWindowFlags {
    s32 flags;
} RoomWindowFlags;

/* The anchor record reached through RoomWindowActor.anchor. */
typedef struct RoomWindowAnchor {
    RoomWindowFlags *marker;      /* 0x00 */
    u8 pad04[0x10];
    s32 speed;                    /* 0x14 */
    u8 pad18[0x2];
    u16 frame;                    /* 0x1A */
    u8 pad1C[0xC];
    s32 pos[3];                   /* 0x28 */
    u8 pad34[0x6];
    u16 yaw;                      /* 0x3A */
} RoomWindowAnchor;

/* The field actor the script object is attached to. */
typedef struct RoomWindowActor {
    u8 pad00[0x28];
    s32 pos[3];                   /* 0x28 */
    u8 pad34[0x64];
    s32 flags;                    /* 0x98 */
    u8 pad9C[0xF0];
    RoomWindowAnchor *anchor;     /* 0x18C */
    u8 pad190[0x58];
    s16 matrix[9];                /* 0x1E8 */
    u8 pad1FA[0x2];
    s32 center[3];                /* 0x1FC */
    u8 pad208[0x48];
    u16 drawFlags;                /* 0x250 */
} RoomWindowActor;

typedef struct RoomWindowTarget {
    u8 pad00[0x28];
    s32 pos[3];                   /* 0x28 */
} RoomWindowTarget;

typedef struct RoomWindowPlayerCore {
    u8 pad00[0x4C];
    s32 flags;                    /* 0x4C */
} RoomWindowPlayerCore;

typedef struct RoomWindowPlayer {
    RoomWindowPlayerCore *core;   /* 0x00 */
    u8 pad04[0x24];
    s32 pos[3];                   /* 0x28 */
} RoomWindowPlayer;

/* Handler state at RoomWindowObject + 0xC (see
 * RoomLib_ConfigureWindowHandler.inc for the configuration ops). */
typedef struct RoomWindowState {
    void (*callback)(void);       /* 0x00 */
    u8 pad04[0xA];
    u8 started;                   /* 0x0E (object + 0x1A) */
    u8 pad0F;
    GteMatrix rotation;           /* 0x10 */
    RoomWindowFixed pos[3];       /* 0x30 */
    s32 pad3C;
    s32 target[3];                /* 0x40 */
    s32 pad4C;
    RoomWindowFixed origin[3];    /* 0x50 */
    s32 pad5C;
    GteShortVector offset;        /* 0x60 */
    GteShortVector angles;        /* 0x68 */
    RoomWindowTarget *targetNode; /* 0x70 */
    s32 travel;                   /* 0x74 */
    s32 step;                     /* 0x78 */
    s32 scale;                    /* 0x7C */
    s32 spinSpeed;                /* 0x80 */
    s32 spin;                     /* 0x84 */
    s16 touched;                  /* 0x88 */
} RoomWindowState;

typedef struct RoomWindowObject {
    u8 pad00[0x4];
    s32 index;                    /* 0x04 */
    RoomWindowActor *actor;       /* 0x08 */
    RoomWindowState state;        /* 0x0C */
} RoomWindowObject;

/* Scratchpad layout used while placing the window. */
typedef struct RoomWindowScratch {
    volatile GteMatrix yaw;       /* 0x00 */
    volatile GteMatrix roll;      /* 0x20 */
    GteVector out;                /* 0x40 */
    GteVector corner;             /* 0x50 */
    GteVector advance;            /* 0x60 */
    union {
        volatile GteShortVector local; /* 0x70: vector fed to the GTE */
        GteShortVector angles;         /* 0x70: RotMatrixYXZ input */
    } in;
    GteShortVector pad78;         /* 0x78 */
    GteShortVector spot;          /* 0x80 */
    GteShortVector edge;          /* 0x88 */
    s16 arrived;                  /* 0x90 */
} RoomWindowScratch;

#define ROOM_WINDOW_SCRATCH ((RoomWindowScratch *)0x1F800000)

extern RoomWindowTrig D_800966EC[];
extern RoomWindowPlayer *g_PlayerEntity;

extern int FieldEng_VecToAngle(s32 *from, s32 *to);
extern int func_800DFC80(s32 *from, s32 *to);
extern int func_800DFC44(int value);
extern int func_800DFE20(s32 *from, s32 *to);
extern void func_800DFE94(s32 *from, s32 *to, GteShortVector *angles);

#endif
