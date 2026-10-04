#ifndef ROOM_M273_BOSS_H
#define ROOM_M273_BOSS_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Boss instance fields read by room_m273's animation controllers. */
typedef struct RoomM273BossOwner {
    u32 flags;
    u8 reserved_04[4];
    s32 sound;
    u8 reserved_0C[0xC];
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

typedef struct RoomM273BossPlayerActor {
    u8 reserved_00[0x4C];
    u32 flags;
} RoomM273BossPlayerActor;

/* 16.16 fixed-point coordinate. */
typedef union RoomM273Fixed {
    s32 value;
    struct {
        signed int fraction : 16;
        signed int integer : 16;
    } parts;
} RoomM273Fixed;

typedef struct RoomM273BossPlayer {
    RoomM273BossPlayerActor *actor;
    u8 reserved_04[0x24];
    RoomM273Fixed location[3];
    u8 reserved_34[0x1C8];
    s32 position[3];
} RoomM273BossPlayer;

typedef struct RoomM273BossFloor {
    u16 value;
} RoomM273BossFloor;

extern RoomM273BossActor *D_800F32D0;
extern RoomM273BossPlayer *g_PlayerEntity;
extern RoomM273BossFloor D_800942EC;
/* Packed sine/cosine table: the low half is the sine, the high half the cosine. */
typedef struct RoomM273BossTrig {
    signed int sine : 16;
    signed int cosine : 16;
} RoomM273BossTrig;

extern RoomM273BossTrig D_800966EC[];

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
    GteShortVector points[2];
    GteShortVector hits[2];
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

/* Falling drop: position (the pad halfword counts frames after landing),
 * ring position, velocity and the player-contact flag. */
typedef struct RoomM273Drop {
    GteShortVector position;
    GteShortVector ring;
    s16 vx;
    s16 vy;
    s16 vz;
    s16 touched;
} RoomM273Drop;

/* Drop splash queue at 0x8019AF04: the contact position, then twelve
 * queued landing positions stored as separate x/y/z arrays. */
typedef struct RoomM273DropQueue {
    GteShortVector hit;
    s16 x[12];
    s16 y[12];
    s16 z[12];
    s16 count;
} RoomM273DropQueue;

extern RoomM273DropQueue D_8019AF04;
extern s16 D_8019AF62;
extern GteShortVector D_8019AB68;
extern u8 D_8019AD54[];
extern u8 D_8019AD58[];
extern u8 D_8019AD5C[];
/* Shard spawned from the sway points toward the player. */
typedef struct RoomM273SwayShard {
    GteShortVector position;
    GteShortVector velocity;
    s8 side;
    u8 frame;
    u8 state;
} RoomM273SwayShard;

typedef struct RoomM273EffectPools {
    u8 reserved_00[8];
    void *pool;
} RoomM273EffectPools;

extern RoomM273EffectPools *D_800F33E0;
extern void *D_8019AF70;
extern GteShortVector D_8019AE10;
extern RenderColor D_8019AE04;
extern RenderColor D_8019AE08;
extern RenderColor D_8019AE0C;
extern u8 D_8019AB70[];
extern u16 D_800E11E8;
extern u16 D_800E11EA;

int func_800CE560(void *pool, int size, int count, void *callback);
int func_800CE5AC(void *list, int base, int size, int count, void *callback);
void *func_800CE610(void *pool);
int func_800CE688(void *list);
int func_800CE78C(void *list);
void Asset_Find08w(int id, int sound, int x, int y, int z);
int func_801981A4(int mode, RoomM273SwayShard *shard);
int func_80198060(int mode, GteShortVector *position);

int Math_IntSqrt(int value);
u16 GetClut(int x, int y);

int FieldEng_VecToAngle(s32 *from, s32 *to);
int FieldEng_TurnToward(s16 current, s16 target, s16 step);

#endif
