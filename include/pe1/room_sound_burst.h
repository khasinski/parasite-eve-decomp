#ifndef PE1_ROOM_SOUND_BURST_H
#define PE1_ROOM_SOUND_BURST_H

#include "common.h"
#include "pe1/gte_types.h"

/* Room effect controllers that play a sound, then emit one particle every
 * third frame along an angle that sweeps half a turn over seventy frames. */

typedef struct RoomSoundBurstTemplate8 {
    s32 word[2];
} __attribute__((packed)) RoomSoundBurstTemplate8;

typedef struct RoomSoundBurstChannel {
    s32 reserved[2];
    char *pool;
} RoomSoundBurstChannel;

typedef struct RoomSoundBurstNode {
    u8 reserved[0x18];
    u8 *state;
} RoomSoundBurstNode;

typedef struct RoomSoundBurstEventState {
    u8 reserved[0xD];
    u8 active;
    u8 reserved0E[4];
    s16 phase;                    /* 0x12 */
} RoomSoundBurstEventState;

typedef struct RoomSoundBurstState {
    u8 reserved[0xA];
    u16 frame;                    /* 0x0A */
    s16 soundHandle;              /* 0x0C */
} RoomSoundBurstState;

typedef struct RoomSoundBurstParticle {
    s16 x, y, z;                  /* 0x00: anchor */
    s16 tilt;                     /* 0x06: random angle for the size cosine */
    s16 wx, wy, wz;               /* 0x08: world position */
    s16 spread;                   /* 0x0E: controller angle, scales the sprite */
    s16 height;                   /* 0x10 */
    u16 phase;                    /* 0x12: sine phase, stepped per frame */
    u16 swing;                    /* 0x14: swing angle, stepped per frame */
    u16 amplitude;                /* 0x16: swing amplitude, stepped per frame */
} RoomSoundBurstParticle;

typedef struct RoomSoundBurstColor {
    u8 r, g, b, code;
} RoomSoundBurstColor;

/* Battle actor record reached through the active battle entity. */
typedef struct RoomSoundBurstActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomSoundBurstActor;

typedef struct RoomSoundBurstBattleEntity {
    RoomSoundBurstActor *actor;
} RoomSoundBurstBattleEntity;

PE1_STATIC_ASSERT(sizeof(RoomSoundBurstParticle) == 0x18,
                  room_sound_burst_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSoundBurstState, soundHandle) == 0xC,
                  room_sound_burst_state_sound_handle);

extern RoomSoundBurstChannel *D_800F32D0, *D_800F33E0;
extern RoomSoundBurstEventState *D_800E2368;
extern s32 D_800E27EC;
extern volatile u16 D_800E11E8;
extern u16 D_800E2850[];
/* Halfword parameter block at 0x800F3368. The controller writes it through
 * volatile scalars; the particle reads it as this struct so the compiler
 * keeps the reads ordered after the particle stores, as retail does. */
typedef struct RoomSoundBurstParams {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomSoundBurstParams;
extern int func_800CE560(void *, int, int, void *);
extern void func_800CE8F0(void *, int, void *, void *);
extern void func_800CE9D4(void *, int, void *);
extern RoomSoundBurstParticle *func_800CE610(void *);
extern int rsin(int);
extern int func_800D3FD8(void);
extern int func_800D3F64(int, int);
extern void func_800866A4(int, int);
extern int func_80071A54(void);
extern RoomSoundBurstBattleEntity *D_8009D254;
extern s16 D_800942EC;
extern u16 D_800E1204[];
extern int D_800F3428;
extern int rcos(int);
extern u16 func_80077AA4(int, int);
extern void func_800CF844(void *, void *, int, void *, int, int);
extern int func_800C6B90(void *position, int radius);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);
extern void func_800CE870(void *object, int mode, void *position);
extern void func_800CFAA8(void *from, void *to, void *angles);
extern void func_800CFD50(void *angles, void *out, int speed);
extern void func_800CFB7C(void *angles, int distance, void *out);
extern int func_80077CF4(int angle);
extern void func_800783E4(void *, void *, int, int, void *);
extern void func_800D2B58(void *, void *, void *, void *, int, int, int);

/* Homing spark: anchor, heading vector, state, timer and sound handle. */
typedef struct RoomHomingSpark {
    s16 x, y, z, reserved06;
    s16 hx, hy, hz, reserved0E;   /* 0x08: heading */
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
    s16 soundHandle;              /* 0x14 */
} RoomHomingSpark;

typedef struct RoomHomingSparkParams {
    s16 distance;                 /* 0x00 */
    s16 reserved02;
    u16 speed;                    /* 0x04 */
} RoomHomingSparkParams;

PE1_STATIC_ASSERT(sizeof(RoomHomingSpark) == 0x16, room_homing_spark_size);

typedef struct RoomSoundBurstMatrixSlot {
    s32 *value;
} RoomSoundBurstMatrixSlot;

extern RoomSoundBurstMatrixSlot D_800BCFA4;
extern void func_800CF3AC(void *track, void *color, int time);
extern void func_800D004C(void *position, int width, int height, int segments,
                          void *rotation, int scale_x, int scale_y,
                          void *color0, void *color1, int intensity, int mode);

#endif
