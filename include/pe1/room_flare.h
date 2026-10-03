#ifndef PE1_ROOM_FLARE_H
#define PE1_ROOM_FLARE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/room_fx.h"

/* Flare effects: a controller that orbits an anchor read from the scene
 * object and sheds drift particles, and a dropped flare particle that falls,
 * flashes the scene on impact and scatters children. */

typedef struct RoomFlareChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomFlareChannel;

typedef struct RoomFlareNode {
    u8 reserved[0x18];
    u8 *state;                    /* 0x18 */
} RoomFlareNode;

typedef struct RoomFlareEventState {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
    u8 reserved0E[4];
    s16 phase;                    /* 0x12 */
} RoomFlareEventState;

typedef struct RoomFlareFrameCounter {
    s16 count;
} RoomFlareFrameCounter;

typedef struct RoomFlareActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomFlareActor;

typedef struct RoomFlareBattleEntity {
    RoomFlareActor *actor;
} RoomFlareBattleEntity;

/* First word of the packet the effect channel's pool points at. */
typedef struct RoomFlarePacket {
    u32 code;
} RoomFlarePacket;

/* Scene object reached through the primary channel pool. */
typedef struct RoomFlareAnchor {
    u8 reserved[0x268];
    s16 x, y, z;                  /* 0x268 */
} RoomFlareAnchor;

typedef struct RoomFlareColor {
    u8 r, g, b, code;
} RoomFlareColor;

typedef struct RoomFlareMatrixSlot {
    s32 *value;
} RoomFlareMatrixSlot;

/* Sprite parameter block at 0x800F3368. */
typedef struct RoomFlareParams {
    u16 parameter00;
    s16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomFlareParams;

/* Orbit flare controller: anchor, state, timer, sound, sweep angle. */
typedef struct RoomOrbitFlareState {
    s16 x, y, z, reserved06;
    s16 state;                    /* 0x08 */
    u16 frame;                    /* 0x0A */
    s16 soundHandle;              /* 0x0C */
    s16 angle;                    /* 0x0E */
    s16 height;                   /* 0x10 */
    s16 spread;                   /* 0x12 */
} RoomOrbitFlareState;

typedef struct RoomOrbitFlareSpawn {
    s32 radius;                   /* 0x00 */
    s32 angle;                    /* 0x04 */
    s32 attached;                 /* 0x08 */
} RoomOrbitFlareSpawn;

/* Dropped flare particle: position, swing, velocity, state, timer. */
typedef struct RoomDroppedFlare {
    s16 x, y, z;                  /* 0x00 */
    s16 swing;                    /* 0x06 */
    s16 vx, vy, vz;               /* 0x08 */
    s16 reserved0E;
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomDroppedFlare;

typedef struct RoomDroppedFlareSpawn {
    s32 reserved00;
    s32 lift;                     /* 0x04 */
} RoomDroppedFlareSpawn;

PE1_STATIC_ASSERT(sizeof(RoomOrbitFlareState) == 0x14, room_orbit_flare_state_size);
PE1_STATIC_ASSERT(sizeof(RoomDroppedFlare) == 0x14, room_dropped_flare_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFlareAnchor, x) == 0x268, room_flare_anchor_x);

extern RoomFlareChannel *D_800F32D0, *D_800F33E0;
extern RoomFlareEventState *D_800E2368;
extern RoomFlareBattleEntity *D_8009D254;
extern RoomFlareFrameCounter D_800942EC;
extern RoomFlareMatrixSlot D_800BCFA4;
extern RoomFlareParams D_800F3368;
extern int D_800E27EC;
extern u16 D_800E1204[];
extern u16 D_800E2850[];
extern int D_800F3428;
extern volatile u16 D_800E11E8;
extern u16 D_800E11E4[];
extern volatile u16 D_800E11EC;
extern u16 D_800E1208;
extern int D_8009D248;
extern u16 D_8009D1CC;

extern int rsin(int angle);
extern int rcos(int angle);
extern int rand(void);
extern u16 GetClut(int x, int y);
extern int func_800D3FD8(void);
extern int func_800D3F64(int, int);
extern void func_800866A4(int, int);
extern int func_800CE560(void *, int, int, void *);
extern void func_800CE8F0(void *, int, void *, void *);
extern void func_800CE9D4(void *, int, void *);
extern void *func_800CE610(void *);
extern int func_800C6B90(void *position, int radius);
extern int func_8001CAB0(int x, int z, int arg2, int arg3);
extern void func_800CEE20(void *position, void *rotation, int scale_x,
                          int scale_y, int texture, int clut, int kind,
                          int fade, void *color);
extern void func_800D1DEC(void *position, void *color, int scale, int flags);

#endif
