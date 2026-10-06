#ifndef PE1_ROOM_SPARK_RING_BURST_H
#define PE1_ROOM_SPARK_RING_BURST_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"

/* The spark ring and spark burst effects linked by ten museum rooms
 * (src/overlays/room_lib/RoomEffect_SparkRingBurst.c). The ring controller
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

/* Texture page index records (0x800E11E8, 0x800E11EC), read as records so
 * they stay ordered after stores through the particle pointer. */
typedef struct RoomSparkTextureSlot {
    u16 index;
} RoomSparkTextureSlot;

extern RoomSparkChannel *D_800F32D0, *D_800F33E0;
extern RoomSparkEventState *D_800E2368;
extern RoomSparkBattleEntity *D_8009D254;
extern RoomSparkTextureSlot D_800E11E8;
extern RoomSparkTextureSlot D_800E11EC;
extern void *D_8009D248;
extern u16 D_8009D1CC;

int func_80071A54(void);
int func_80077AA4(int x, int y);
void *func_800CE610(char *pool);
int func_800CE560(char *pool, int size, int count, void *callback);
void func_800CE8F0(char *pool, int joint, void *offset, void *out);
int func_800C6B90(void *position, int radius);
int func_8001CAB0(int x, int z, void *vertices, int count);
int func_800D3FD8(void);
int func_800D3F64(int sound, int handle);

/* Per-room data: the colour ramp of the ring sprite and the damped sparks,
 * and the word the room's slot setter writes. */
extern u8 g_RoomSparkRingColorRamp[];
extern int g_RoomSparkSlot;

#endif
