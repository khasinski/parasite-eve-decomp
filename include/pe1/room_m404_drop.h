#ifndef PE1_ROOM_M404_DROP_H
#define PE1_ROOM_M404_DROP_H

#include "common.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/floor_polygon.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/render_prim.h"
#include "pe1/room_floor.h"

/* room_m404 sinking drop (RoomM404_SinkingDropCallback): a drop that sinks and jitters,
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

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern RoomM404DropRotation g_RoomM404DropFanTilt;
extern RoomM404DropColor g_RoomM404DropFanColor;
extern RoomM404DropColor g_RoomM404DropGlowColor;
extern RoomM404DropRotation g_RoomM404DropFanTilt;
extern RoomM404DropColor g_RoomM404DropFanColor;
extern RoomM404DropColor g_RoomM404DropGlowColor;
extern u8 g_RoomM404SinkingDropColorTrack[];

int rand(void);

#endif
