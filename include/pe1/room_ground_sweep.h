#ifndef PE1_ROOM_GROUND_SWEEP_H
#define PE1_ROOM_GROUND_SWEEP_H

#include "common.h"
#include "pe1/gte_types.h"

/* Ground sweep controller state: the sweeping head, its heading, the trail
 * buffer the ribbon draw keeps, and the phase machine. */
typedef struct RoomGroundSweep {
    s16 x, y, z, reserved06;      /* 0x00 */
    GteShortVector heading;       /* 0x08 */
    u8 trail[0xA0];               /* 0x10: ribbon history */
    s16 phase;                    /* 0xB0 */
    u16 timer;                    /* 0xB2 */
    s16 sweeps;                   /* 0xB4: sweeps left before the fade */
    s16 soundHandle;              /* 0xB6 */
} RoomGroundSweep;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomGroundSweep, phase) == 0xB0,
                  room_ground_sweep_phase);

typedef struct RoomGroundSweepParams {
    s32 distance;
} RoomGroundSweepParams;

/* Scorch mark the head leaves behind: a position and two cleared words. */
typedef struct RoomGroundSweepMark {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 blocked;                  /* 0x08 */
    u16 frame;                    /* 0x0A */
} RoomGroundSweepMark;

/* First object of the primary channel's pool: its packet flags. */
typedef struct RoomGroundSweepHitObject {
    u32 flags;
} RoomGroundSweepHitObject;

typedef struct RoomGroundSweepHitPool {
    RoomGroundSweepHitObject *object;
} RoomGroundSweepHitPool;

typedef struct RoomGroundSweepActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomGroundSweepActor;

typedef struct RoomGroundSweepBattleEntity {
    RoomGroundSweepActor *actor;
} RoomGroundSweepBattleEntity;

#endif
