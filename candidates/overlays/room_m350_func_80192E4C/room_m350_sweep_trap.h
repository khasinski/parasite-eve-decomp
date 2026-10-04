#ifndef PE1_ROOM_M350_SWEEP_TRAP_H
#define PE1_ROOM_M350_SWEEP_TRAP_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* room_m350 sweeping beam trap (func_80192E4C): a column of sprites swept
 * down from the actor's head, a flare and two fan blades towards the
 * anchor, and a quad test that tags the player once. */
typedef struct RoomM350SweepTrap {
    /* 0x00 */ GteShortVector *anchor;
    /* 0x04 */ s16 done;
} RoomM350SweepTrap;

typedef struct RoomM350TrapNode {
    u32 flags;
} RoomM350TrapNode;

typedef struct RoomM350TrapActor {
    /* 0x00 */ RoomM350TrapNode *node;
    /* 0x04 */ u8 reserved04[0x36];
    /* 0x3A */ s16 heading;
} RoomM350TrapActor;

typedef struct RoomM350TrapChannel {
    s32 reserved[2];
    RoomM350TrapActor *actor;
} RoomM350TrapChannel;

typedef struct RoomM350PlayerActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM350PlayerActor;

typedef struct RoomM350Player {
    /* 0x000 */ RoomM350PlayerActor *actor;
    /* 0x004 */ u8 reserved004[0xA];
    /* 0x00E */ u8 stance;
    /* 0x00F */ u8 reserved00F[0x1ED];
    /* 0x1FC */ s16 x;
    /* 0x1FE */ u8 reserved1FE[6];
    /* 0x204 */ s32 z;
} RoomM350Player;

/* rcossin table pairs. */
typedef struct RoomM350Trig {
    s16 sin;
    s16 cos;
} RoomM350Trig;

typedef struct RoomM350FloorLevel {
    s16 count;
} RoomM350FloorLevel;

extern RoomM350Trig D_800966EC[];
extern RoomM350TrapChannel *D_800F32D0;
extern RoomM350Player *g_PlayerEntity;
extern RoomM350FloorLevel D_800942EC;
extern RenderColor D_8019A434[];
extern RenderColor D_8019A3C8;
extern RenderColor D_8019A43C;
extern RenderColor D_8019A440;
extern GteShortVector D_8019A444[];
extern u8 D_8019A79C;
extern u16 D_800E11EA;
extern u16 D_800E11FA;
extern u16 D_800E120A;
u16 GetClut(int x, int y);
int Math_IntSqrt(int value);
int rsin(int angle);
int rcos(int angle);

#endif
