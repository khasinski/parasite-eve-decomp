#ifndef PE1_ROOM_SPARK_RING_BURST_H
#define PE1_ROOM_SPARK_RING_BURST_H

#include "common.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/floor_polygon.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/room_interface.h"

/* The spark ring and spark burst effects linked by ten museum rooms
 * (src/overlays/room_lib/spark_ring_burst.c). The ring controller
 * plays a sound, then sheds damped sparks around the actor while it draws
 * a pulsing sprite with two expanding rings; the burst spawner launches
 * homing sparks that shed trail sparks and burst into debris when they
 * reach the actor, the floor or a wall. */

/* Damped spark shed by the ring controller. */
typedef struct RoomSparkRingParticle {
    s16 x, y, z, size;            /* 0x00 */
    s16 vx, vy, vz, angle;        /* 0x08 */
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
} RoomSparkRingParticle;

PE1_STATIC_ASSERT(sizeof(RoomSparkRingParticle) == 0x14,
                  room_spark_ring_particle_size);

typedef struct RoomSparkRingState {
    s16 state;                    /* 0x00 */
    s16 timer;                    /* 0x02 */
    GteShortVector target;        /* 0x04 */
} RoomSparkRingState;

/* Burst spark: a damped spark whose heading block is also drawn as a
 * rotation once the particle settles, so the word after vz holds one. */
typedef struct RoomBurstSpark {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 vw;                       /* 0x0E: one while the heading is a rotation */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomBurstSpark;

PE1_STATIC_ASSERT(sizeof(RoomBurstSpark) == 0x14, room_burst_spark_size);

/* Eight-byte seed the effects copy into a local before use. */
typedef struct RoomSparkSeed {
    u8 bytes[8];
} RoomSparkSeed;

/* Position and heading the burst spawner reads from the actor's joints. */
typedef struct RoomSparkLaunch {
    GteShortVector position;      /* 0x00 */
    s16 hx, hy, hz;               /* 0x08 */
} RoomSparkLaunch;

/* Field effect channel: its pool sits at 0x08. */
typedef struct RoomSparkNode {
    u8 reserved[0x18];
    u8 *state;                    /* 0x18 */
} RoomSparkNode;

typedef struct RoomSparkChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomSparkChannel;

typedef struct RoomSparkEventState {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
} RoomSparkEventState;

typedef struct RoomSparkActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomSparkActor;

typedef struct RoomSparkBattleEntity {
    RoomSparkActor *actor;
} RoomSparkBattleEntity;

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;

int rand(void);

/* Per-room data: the colour ramp of the ring sprite and the damped sparks,
 * and the word the room's slot setter writes. */
extern u8 g_RoomSparkRingColorRamp[];
extern int g_RoomSparkSlot;

#endif
