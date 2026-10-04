#ifndef PE1_ROOM_M404_DROP_H
#define PE1_ROOM_M404_DROP_H

#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"

/* room_m404 sinking drop (func_80192540): a drop that sinks and jitters,
 * spreads on the floor and sheds splash children, a lingering pool that
 * spits droplets, and their bouncing splashes; each flashes the scene when
 * it reaches the player. */
typedef struct RoomM404Drop {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ s16 hazard;
    /* 0x08 */ s16 vx, vy, vz;
    /* 0x0E */ s16 pad0E;
    /* 0x10 */ s16 state;
    /* 0x12 */ s16 timer;
} RoomM404Drop;

typedef struct RoomM404DropRotation {
    s16 x, y, z, flags;
} __attribute__((packed)) RoomM404DropRotation;

typedef struct RoomM404DropColor {
    u8 r, g, b, code;
} __attribute__((packed)) RoomM404DropColor;

typedef struct RoomM404DropNode {
    u32 flags;
} RoomM404DropNode;

typedef struct RoomM404DropPool {
    RoomM404DropNode *node;
} RoomM404DropPool;

typedef struct RoomM404DropChannel {
    s32 reserved[2];
    RoomM404DropPool *pool;
} RoomM404DropChannel;

typedef struct RoomM404DropSpawnChannel {
    s32 reserved[2];
    void *pool;
} RoomM404DropSpawnChannel;

typedef struct RoomM404DropActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM404DropActor;

typedef struct RoomM404DropEntity {
    RoomM404DropActor *actor;
} RoomM404DropEntity;

typedef struct RoomM404DropEvent {
    u8 reserved[0xD];
    u8 active;
} RoomM404DropEvent;

/* The sprite parameter block at 0x800F3368 (RenderEffectParameters). */
typedef struct RoomM404DropParams {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomM404DropParams;

typedef struct RoomM404DropMatrixSlot {
    s32 *value;
} RoomM404DropMatrixSlot;

extern RoomM404DropParams D_800F3368;
extern RoomM404DropMatrixSlot D_800BCFA4;
extern RoomM404DropChannel *D_800F32D0;
extern RoomM404DropSpawnChannel *D_800F33E0;
extern RoomM404DropEvent *D_800E2368;
extern RoomM404DropEntity *D_8009D254;
extern RoomM404DropRotation D_8018F1CC;
extern RoomM404DropColor D_8018F1D4;
extern RoomM404DropColor D_8018F1D8;
extern u8 D_80193EFC[];
extern u16 D_800E1204[];
extern int D_800F3428;
extern int D_800E27EC;
/* The floor height read as a one-field record so the loads stay below the
 * particle stores, as retail has them. */
typedef struct RoomM404DropFloor {
    s16 y;
} RoomM404DropFloor;

extern RoomM404DropFloor D_800942EC;
extern int D_8009D248;
extern u16 D_8009D1CC;

int func_80071A54(void);
RoomM404Drop *func_800CE610(void *pool);
int func_800C6B90(void *position, int radius);
int func_8001CAB0(int x, int z, int floor, int count);
int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 GetClut(int x, int y);
void func_800CEE20(void *position, RoomM404DropRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RoomM404DropColor *color);
void func_800D004C(void *position, int width, int height, int segments,
                   RoomM404DropRotation *rotation, int scale_x, int scale_y,
                   RoomM404DropColor *color0, RoomM404DropColor *color1,
                   int intensity, int mode);
void func_800CF3AC(void *track, RoomM404DropColor *color, int time);

#endif
