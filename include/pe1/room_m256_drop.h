#ifndef PE1_ROOM_M256_DROP_H
#define PE1_ROOM_M256_DROP_H

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

/* room_m256 scatter drop (RoomM256_ScatterDropCallback): a falling drop that jitters,
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

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern GteRotation g_RoomM256SpriteTilt;
extern RoomM256DropColor g_RoomM256DropColor;
extern GteRotation g_RoomM256SpriteTilt;
extern RoomM256DropColor g_RoomM256DropColor;

int rand(void);

#endif
