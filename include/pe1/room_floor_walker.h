#ifndef PE1_ROOM_FLOOR_WALKER_H
#define PE1_ROOM_FLOOR_WALKER_H

#include "common.h"
#include "pe1/gte_types.h"

/* Room creature that walks along its heading inside the walkable floor
 * polygon, turning off the polygon's edges, and hops with a simple
 * vertical velocity/gravity integration. */

typedef struct RoomFloorWalkerCore {
    u8 pad00[0xE];
    u8 phase;                     /* 0x0E */
    u8 pad0F[0x19];
    s32 pos[3];                   /* 0x28: 16.16 */
    u16 pad34[3];
    u16 heading;                  /* 0x3A */
    u8 pad3C[0x30];
    s32 field6C;                  /* 0x6C */
    u8 pad70[0x1C];
    s32 field8C;                  /* 0x8C */
    u8 pad90[0x8];
    u32 flags;                    /* 0x98 */
} RoomFloorWalkerCore;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFloorWalkerCore, flags) == 0x98,
                  room_floor_walker_core_flags);

/* Handler state at RoomFloorWalkerObject + 0xC. */
typedef struct RoomFloorWalkerState {
    void (*callback)(void);       /* 0x00 */
    u8 pad04[0xA];
    u8 started;                   /* 0x0E (object + 0x1A) */
    u8 pad0F[5];
    s32 speed;                    /* 0x14: 16.16 */
    s32 hopVelocity;              /* 0x18 */
    s32 velocity;                 /* 0x1C */
    s32 gravity;                  /* 0x20 */
    u16 angle;                    /* 0x24 */
    s16 rest;                     /* 0x26: frames left on the floor */
    u16 restTime;                 /* 0x28 */
    s16 turning;                  /* 0x2A: last step hit an edge */
} RoomFloorWalkerState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFloorWalkerState, turning) == 0x2A,
                  room_floor_walker_state_turning);

typedef struct RoomFloorWalkerObject {
    u8 pad00[0x8];
    RoomFloorWalkerCore *core;    /* 0x08 */
    RoomFloorWalkerState state;   /* 0x0C */
} RoomFloorWalkerObject;

/* Scratchpad work area for one step. */
typedef struct RoomFloorWalkerScratch {
    union {
        GteShortVector v;
        struct {
            s32 xy;
            s16 z;
        } words;
    } step;                       /* 0x00: forward step fed to the GTE */
    GteVector rotated;            /* 0x08 */
    GteVector next;               /* 0x18: candidate 16.16 position */
    volatile GteMatrix yaw;       /* 0x28 */
    s32 area;                     /* 0x48 */
} RoomFloorWalkerScratch;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFloorWalkerScratch, area) == 0x48,
                  room_floor_walker_scratch_area);

#define ROOM_FLOOR_WALKER_SCRATCH ((RoomFloorWalkerScratch *)0x1F800000)

/* Walkable polygon vertex: x and the packed y/z word. */
typedef struct RoomFloorWalkerVertex {
    s16 pad00;
    u16 x;                        /* 0x02 */
    u32 yz;                       /* 0x04 */
} RoomFloorWalkerVertex;

/* rsin/rcos table entry, also read as one word. */
typedef union RoomFloorWalkerTrig {
    s32 word;
    struct {
        s16 sine;
        s16 cosine;
    } part;
} RoomFloorWalkerTrig;

typedef struct RoomFloorWalkerTarget {
    u8 pad00[0x28];
    s32 pos[3];                   /* 0x28 */
} RoomFloorWalkerTarget;

extern RoomFloorWalkerTrig D_800966EC[];
extern RoomFloorWalkerVertex *D_8009D248;
extern s16 D_8009D1CC;
/* Floor height, read as a one-field record so the load stays below the
 * position store, as retail does. */
typedef struct RoomFloorWalkerFloor {
    s16 y;
} RoomFloorWalkerFloor;

extern RoomFloorWalkerFloor D_800942EC;
extern RoomFloorWalkerTarget *D_8009D254; /* player entity */

extern int FieldEng_VecToAngle(s32 *from, s32 *to);

#endif
