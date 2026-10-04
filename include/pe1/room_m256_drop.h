#ifndef PE1_ROOM_M256_DROP_H
#define PE1_ROOM_M256_DROP_H

#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"

/* room_m256 scatter drop (func_80192844): a falling drop that jitters,
 * sheds three kinds of splash children, flashes the scene when it reaches
 * the player and expires when it leaves the walkable area. */
typedef struct RoomM256Drop {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ s16 hazard;
    /* 0x08 */ s16 vx, vy, vz;
    /* 0x0E */ s16 pad0E;
    /* 0x10 */ s16 state;
    /* 0x12 */ s16 timer;
} RoomM256Drop;

typedef struct RoomM256DropRotation {
    s16 x, y, z, flags;
} __attribute__((packed)) RoomM256DropRotation;

typedef struct RoomM256DropColor {
    u8 r, g, b, code;
} __attribute__((packed)) RoomM256DropColor;

typedef struct RoomM256DropNode {
    u32 flags;
} RoomM256DropNode;

typedef struct RoomM256DropPool {
    RoomM256DropNode *node;
} RoomM256DropPool;

typedef struct RoomM256DropChannel {
    s32 reserved[2];
    RoomM256DropPool *pool;
} RoomM256DropChannel;

typedef struct RoomM256DropSpawnChannel {
    s32 reserved[2];
    void *pool;
} RoomM256DropSpawnChannel;

typedef struct RoomM256DropActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM256DropActor;

typedef struct RoomM256DropEntity {
    RoomM256DropActor *actor;
} RoomM256DropEntity;

typedef struct RoomM256DropEvent {
    u8 reserved[0xD];
    u8 active;
} RoomM256DropEvent;

/* The sprite parameter block at 0x800F3368 (RenderEffectParameters). */
typedef struct RoomM256DropParams {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomM256DropParams;

typedef struct RoomM256DropMatrixSlot {
    s32 *value;
} RoomM256DropMatrixSlot;

extern RoomM256DropParams D_800F3368;
extern RoomM256DropMatrixSlot D_800BCFA4;
extern RoomM256DropChannel *D_800F32D0;
extern RoomM256DropSpawnChannel *D_800F33E0;
extern RoomM256DropEvent *D_800E2368;
extern RoomM256DropEntity *D_8009D254;
extern RoomM256DropRotation D_8018F1CC;
extern RoomM256DropColor D_8018F1D4;
extern u16 D_800E1204[];
extern int D_800F3428;
/* The floor height read as a one-field record so the loads stay below the
 * particle stores, as retail has them. */
typedef struct RoomM256DropFloor {
    s16 y;
} RoomM256DropFloor;

extern RoomM256DropFloor D_800942EC;
extern int D_8009D248;
extern u16 D_8009D1CC;

int func_80071A54(void);
RoomM256Drop *func_800CE610(void *pool);
int func_800C6B90(void *position, int radius);
int func_8001CAB0(int x, int z, int floor, int count);
int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 GetClut(int x, int y);
void func_800CEE20(void *position, RoomM256DropRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RoomM256DropColor *color);
void func_800D004C(void *position, int width, int height, int segments,
                   RoomM256DropRotation *rotation, int scale_x, int scale_y,
                   RoomM256DropColor *color0, RoomM256DropColor *color1,
                   int intensity, int mode);
void func_800D1DEC(void *position, RoomM256DropColor *color, int scale, int flags);

#endif
