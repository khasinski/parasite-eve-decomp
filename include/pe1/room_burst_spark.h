#ifndef PE1_ROOM_BURST_SPARK_H
#define PE1_ROOM_BURST_SPARK_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Burst spark particle family (room_m203 and siblings): a homing spark
 * that sheds trail sparks, bursts into debris at the actor, the floor or a
 * wall, and draws each state with the shared sprite parameter block. */

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

typedef struct RoomBurstSparkChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomBurstSparkChannel;

typedef struct RoomBurstSparkEventState {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
} RoomBurstSparkEventState;

typedef struct RoomBurstSparkActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomBurstSparkActor;

typedef struct RoomBurstSparkBattleEntity {
    RoomBurstSparkActor *actor;
} RoomBurstSparkBattleEntity;

/* Floor height counter at 0x800942EC, read as a record so the compare
 * stays after the particle's velocity store. */
typedef struct RoomBurstSparkFloor {
    s16 height;
} RoomBurstSparkFloor;

/* Texture page and palette index records (0x800E11E8, 0x800E11EC,
 * 0x800E1208, 0x800E120C), read as records so they stay ordered after
 * stores through the particle pointer. */
typedef struct RoomBurstSparkTextureSlot {
    u16 index;
} RoomBurstSparkTextureSlot;

extern RoomBurstSparkChannel *D_800F32D0, *D_800F33E0;
extern RoomBurstSparkEventState *D_800E2368;
extern RoomBurstSparkBattleEntity *D_8009D254;
extern RoomBurstSparkFloor D_800942EC;
extern RoomBurstSparkTextureSlot D_800E11E8;
extern RoomBurstSparkTextureSlot D_800E11EC;
extern u16 D_800E1208;
extern u16 D_800E120C;
extern void *D_8009D248;
extern u16 D_8009D1CC;

extern int func_80071A54(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern u16 func_80077AA4(int x, int y);
extern void *func_800CE610(void *pool);
extern int func_800C6B90(void *position, int radius);
extern int func_8001CAB0(int x, int z, void *vertices, int count);

#endif
