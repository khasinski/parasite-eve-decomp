#ifndef PE1_ROOM_PHASED_SPARK_H
#define PE1_ROOM_PHASED_SPARK_H

#include "common.h"
#include "pe1/gte_types.h"

/* Phased spark: a damped spark with a sub-phase word, used by the sparks
 * that drop from above the actor, land on the floor and flash the scene.
 * This header stands alone so the particle record can carry its own
 * parameter-block and channel types. */

#ifndef PE1_ROOM_PHASED_SPARK_TYPE
#define PE1_ROOM_PHASED_SPARK_TYPE
typedef struct RoomPhasedSpark {
    s16 x, y, z;                  /* 0x00 */
    u16 angle;                    /* 0x06 */
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 reserved0E;
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
    s16 phase;                    /* 0x14 */
    s16 reserved16;
} RoomPhasedSpark;
#endif

PE1_STATIC_ASSERT(sizeof(RoomPhasedSpark) == 0x18, room_phased_spark_size);

typedef struct RoomPhasedSparkFrameCounter {
    s16 count;
} RoomPhasedSparkFrameCounter;

typedef struct RoomPhasedSparkChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomPhasedSparkChannel;

typedef struct RoomPhasedSparkEventState {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
} RoomPhasedSparkEventState;

typedef struct RoomPhasedSparkActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomPhasedSparkActor;

typedef struct RoomPhasedSparkBattleEntity {
    RoomPhasedSparkActor *actor;
} RoomPhasedSparkBattleEntity;

/* First word of the packet the effect channel's pool points at. */
typedef struct RoomPhasedSparkPacket {
    u32 code;
} RoomPhasedSparkPacket;

extern RoomPhasedSparkFrameCounter D_800942EC;
extern int D_800E27EC;
extern u16 D_800E1204[];
extern int D_800F3428;
extern RoomPhasedSparkChannel *D_800F32D0, *D_800F33E0;
extern RoomPhasedSparkEventState *D_800E2368;
extern RoomPhasedSparkBattleEntity *D_8009D254;

/* Sprite parameter block at 0x800F3368. */
typedef struct RoomPhasedSparkParams {
    u16 parameter00;
    s16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomPhasedSparkParams;

extern RoomPhasedSparkParams D_800F3368;

extern int func_80071A54(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern u16 func_80077AA4(int, int);
extern RoomPhasedSpark *func_800CE610(void *pool);
extern void func_800CE870(void *object, int mode, void *position);
extern int func_800C6B90(void *, int);
extern void func_800CEE20(GteShortVector *position, GteRotation *rotation,
                          int scale_x, int scale_y, int intensity, int clut,
                          int kind, int fade, void *color);

#endif
