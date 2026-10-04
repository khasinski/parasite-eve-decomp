#ifndef ROOM_M273_BOSS_H
#define ROOM_M273_BOSS_H

#include "common.h"
#include "pe1/gte_types.h"

/* Boss instance fields read by room_m273's animation controllers. */
typedef struct RoomM273BossOwner {
    u32 flags;
    u8 reserved_04[0x14];
    u8 *status;
} RoomM273BossOwner;

typedef struct RoomM273BossInstance {
    RoomM273BossOwner *owner;
    u8 reserved_04[0x0A];
    u8 animation;
    u8 frame_count;
    u8 reserved_10[4];
    union {
        s32 fixed;
        struct {
            u16 fraction;
            u16 frame;
        } parts;
    } time;
    u8 reserved_18[2];
    u16 frame_1A;
    u8 reserved_1C[0x10];
    s32 height;
    u8 reserved_30[0x0A];
    s16 yaw;
    u8 reserved_3C[0x1C0];
    s32 position[3];
    u8 reserved_208[0x30];
    GteMatrix *transforms;
} RoomM273BossInstance;

typedef struct RoomM273BossActor {
    u8 reserved_00[8];
    RoomM273BossInstance *instance;
} RoomM273BossActor;

typedef struct RoomM273BossPlayer {
    u8 reserved_00[0x1FC];
    s32 position[3];
} RoomM273BossPlayer;

typedef struct RoomM273BossFloor {
    u16 value;
} RoomM273BossFloor;

extern RoomM273BossActor *D_800F32D0;
extern RoomM273BossPlayer *g_PlayerEntity;
extern RoomM273BossFloor D_800942EC;
extern s16 D_800966EE[];

/* Boss controller state at 0x8019AE9C. */
typedef struct RoomM273BossState {
    GteShortVector hands[2];
    u8 reserved_10[6];
    s16 reserved_16;
    s32 base_height;
    u8 reserved_1C[0x30];
    s16 reserved_4C;
    s16 repeat;
    s16 floor;
    s16 spin_target;
    s16 spin;
    s16 spin_step;
    s16 tick;
    s16 tick_limit;
    u8 done;
    u8 reserved_5D;
    u8 spinning;
} RoomM273BossState;

extern RoomM273BossState D_8019AE9C;

/* Sway controller state at 0x8019AF74: four transformed points (the pad
 * halfword is a per-point flag) and the animation snapshot. */
typedef struct RoomM273SwayState {
    GteShortVector points[4];
    s16 animation;
    s16 frame;
    s16 frame_1A;
    s16 repeat;
    s16 sway_timer;
    s16 sway_step;
    s16 cooldown;
    u8 done;
} RoomM273SwayState;

extern RoomM273SwayState D_8019AF74;
extern s16 D_800966EC[];

int FieldEng_VecToAngle(s32 *from, s32 *to);
int FieldEng_TurnToward(s16 current, s16 target, s16 step);

#endif
