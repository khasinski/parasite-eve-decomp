#ifndef PE1_ROOM_RISING_SPRAY_H
#define PE1_ROOM_RISING_SPRAY_H

#include "common.h"
#include "pe1/gte_types.h"

/* Rising spray spark: a damped spark whose swirl angle is signed. */
typedef struct RoomSpraySpark {
    s16 x, y, z;                  /* 0x00 */
    s16 swirl;                    /* 0x06: swirl angle around the path */
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 reserved0E;
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomSpraySpark;

PE1_STATIC_ASSERT(sizeof(RoomSpraySpark) == 0x14, room_spray_spark_size);

/* First object of the primary channel's pool: its packet flags. */
typedef struct RoomSprayHitObject {
    u32 flags;
} RoomSprayHitObject;

typedef struct RoomSprayHitPool {
    RoomSprayHitObject *object;
} RoomSprayHitPool;

typedef struct RoomSprayActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomSprayActor;

typedef struct RoomSprayBattleEntity {
    RoomSprayActor *actor;
} RoomSprayBattleEntity;

#endif
