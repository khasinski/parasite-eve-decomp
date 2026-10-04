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
        struct {
            unsigned int fraction : 16;
            unsigned int frame : 16;
        } bits;
    } time;
    union {
        s32 fixed;
        struct {
            u16 fraction;
            u16 frame;
        } parts;
        struct {
            unsigned int fraction : 16;
            unsigned int frame : 16;
        } bits;
    } previous;
    u8 reserved_1C[0x0C];
    s32 x;
    s32 height;
    s32 z;
    u8 reserved_34[0x06];
    s16 yaw;
    u8 reserved_3C[0x1C0];
    s32 position[3];
    u8 reserved_208[0x30];
    GteMatrix *transforms;
    u8 reserved_23C[0x1A];
    s16 attack_angle;
    u8 reserved_258[2];
    u8 attack_kind;
    u8 attack_power;
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
    u8 reserved_04[0x0A];
    u8 mode;
    u8 reserved_0F[0x19];
    RoomM273Fixed location[3];
    u8 reserved_34[0x1C8];
    s32 position[3];
    u8 reserved_208[0x30];
    GteMatrix *transforms;
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
    s16 hit[3];       /* 0x10: where a falling trail touched the player */
    s16 hit_flag;
    s32 base_height;
    s16 landing_x[8]; /* 0x1C: trail landing positions */
    s16 landing_y[8];
    s16 landing_z[8];
    s16 landing_count;
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
    /* Sweep attack state (animation 13). */
    s16 loops;        /* 0x52: loops left */
    s16 reserved_54;
    s16 side;         /* 0x56: sweep step table index */
    s16 divisor;      /* 0x58 */
    s16 step;         /* 0x5A */
    s16 target;       /* 0x5C: sweep target angle */
    s16 floor;        /* 0x5E */
    s16 contact;      /* 0x60: contact frame */
    s16 turn;         /* 0x62: turn step */
    u8 in_contact;    /* 0x64 */
    u8 done;
    u8 in_window;
    u8 started;
    u8 *model;        /* 0x68: ground ring model */
} RoomM273DropQueue;

extern RoomM273DropQueue D_8019AF04;
extern s16 D_8019AF62;

/* Sweep attack frame windows at 0x8019AD80, one per side. */
typedef struct RoomM273SweepStep {
    s16 start;        /* animation frame set on entry */
    s16 loop_start;   /* frame the loop rewinds to; sweep window start */
    s16 loop_end;     /* sweep window end */
    s16 end;          /* animation done */
    s16 scale;        /* sweep angle per step (times the step) */
    s16 offset;       /* sweep angle offset */
} RoomM273SweepStep;

extern RoomM273SweepStep D_8019AD80[];
extern GteShortVector D_8019AEFC; /* transformed sweep point */
int func_80079FB4(int x, int z);

/* Ground ring (func_80198E94): model draw helpers and the ring colours. */
extern RenderColor D_8019AE1C[2];
extern u16 D_800E11FA;
extern u16 D_800E120A;
u16 GetTPage(int tp, int abr, int x, int y);
void GsSetOrign(int tpage, int clut);
void func_800C6ED8(int mode);
void func_800C6EF8(u8 *model);
void func_800C6FA0(u8 *model, int depth);
void func_800C71E4(u8 *model, GteMatrix *matrix);
void func_800C6F4C(u8 *model);
void *memset(void *dst, int value, unsigned int size);
int func_8005186C(int value);
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

/* Falling trail (func_80194E6C): 100-byte pool record that spins down to
 * the floor and keeps an eight-entry ring of two-point trail samples. */
typedef struct RoomM273FallingTrail {
    GteShortVector position;
    s16 pitch;
    s16 yaw;
    s16 spin;
    s16 landed;
    s16 trail_x[16];
    s16 trail_z[16];
    s16 trail_y[8];
    s16 head;
    s16 count;
} RoomM273FallingTrail;

extern GteShortVector D_8019ACA4; /* drop offset */
extern GteShortVector D_8019ACAC; /* trail sample offsets */
extern GteShortVector D_8019ACB4;
extern RenderColor D_8019ACBC;
void func_800CEE20(GteShortVector *position, GteRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RenderColor *color);

int FieldEng_VecToAngle(s32 *from, s32 *to);
int FieldEng_TurnToward(s16 current, s16 target, s16 step);

#endif
