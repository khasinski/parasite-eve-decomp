#ifndef PE1_ROOM_SPARK_H
#define PE1_ROOM_SPARK_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Room spark particles: damped velocity, gravity and a floor bounce against
 * the frame-count driven floor height. */

typedef struct RoomDampedSpark {
    s16 x, y, z;                  /* 0x00 */
    u16 angle;                    /* 0x06: spin seed, bit 0 halves the size */
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 reserved0E;
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomDampedSpark;

PE1_STATIC_ASSERT(sizeof(RoomDampedSpark) == 0x14, room_damped_spark_size);

/* Frame counter record at 0x800942EC; reading it as a record keeps the
 * compare ordered after the particle's velocity store, as retail does. */
typedef struct RoomSparkFrameCounter {
    s16 count;
} RoomSparkFrameCounter;

extern RoomSparkFrameCounter D_800942EC;
/* The same counter under its other label, read unsigned as a record so a
 * store through a particle pointer keeps it from being hoisted. */
typedef struct RoomSparkFrameTick {
    u16 count;
} RoomSparkFrameTick;

extern RoomSparkFrameTick g_FrameCount16;
extern int rsin(int angle);
extern int rcos(int angle);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern u16 func_80077AA4(int, int);
extern void func_800D2104(void *position, void *color, int size, int alpha);

/* Particle pools reached through the two effect channels. */
typedef struct RoomSparkChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomSparkChannel;

extern RoomSparkChannel *D_800F32D0, *D_800F33E0;
extern u16 D_800E11EA;
extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomDampedSpark *func_800CE610(void *pool);
extern int func_80071A54(void);
extern int func_800D3FD8(void);
extern int func_800D3F64(int sound, int handle);
extern void *D_800B0E64;
extern void func_8006DF50(void *channel, int id, int value, int volume, int pan);

/* Ring spawner parameters: centre and the ring radius. */
typedef struct RoomSparkRingParams {
    s16 x, y, z, reserved06;
    s32 radius;                   /* 0x08 */
} RoomSparkRingParams;

/* Phased spark: a damped spark with a sub-phase word. */
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

extern void func_800CE8F0(void *pool, int index, void *rotation, void *position);

/* Comet spark: a damped spark that carries two steering rotations. */
typedef struct RoomCometSpark {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 reserved0E;
    GteRotation steer;            /* 0x10 */
    GteRotation drift;            /* 0x18 */
    s16 state;                    /* 0x20 */
    u16 timer;                    /* 0x22 */
} RoomCometSpark;

PE1_STATIC_ASSERT(sizeof(RoomCometSpark) == 0x24, room_comet_spark_size);

/* Anchor the comet spawner keeps: a position and a burst countdown. */
typedef struct RoomCometSparkAnchor {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 count;                    /* 0x08 */
} RoomCometSparkAnchor;

typedef struct RoomSparkEventState {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
} RoomSparkEventState;

/* First node of the primary channel's pool: its state byte. */
typedef struct RoomSparkNode {
    u8 reserved[0x18];
    u8 *state;
} RoomSparkNode;

extern RoomSparkEventState *D_800E2368;
extern u16 D_800E11FA;
extern RoomCometSpark *func_800CE610_comet(void *pool) __asm__("func_800CE610");

/* Bouncing spark: the damped spark layout with a rise acceleration in the
 * word the damped spark leaves reserved. */
typedef struct RoomBounceSpark {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 ay;                       /* 0x0E: rise acceleration */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomBounceSpark;

PE1_STATIC_ASSERT(sizeof(RoomBounceSpark) == 0x14, room_bounce_spark_size);

extern RoomBounceSpark *func_800CE610_bounce(void *pool) __asm__("func_800CE610");

/* Beam spark: a line drawn from the actor to a tip that is blended towards
 * the target, with a scaled model drawn at the target. */
typedef struct RoomBeamSpark {
    s16 x, y, z, reserved06;      /* 0x00: actor position */
    s16 tipX, tipY, tipZ;         /* 0x08: beam tip */
    s16 reserved0E;
    s16 tx, ty, tz;               /* 0x10: target */
    s16 reserved16;
    s16 state;                    /* 0x18 */
    u16 timer;                    /* 0x1A */
    s16 brightness;               /* 0x1C: beam colour, zero hides the beam */
    s16 scale;                    /* 0x1E: model scale */
    u16 glow;                     /* 0x20: model brightness, zero hides it */
    s16 countdown;                /* 0x22: frames until the next hit */
} RoomBeamSpark;

PE1_STATIC_ASSERT(sizeof(RoomBeamSpark) == 0x24, room_beam_spark_size);

/* Beam spawner parameters: the target, the beam duration and hit interval. */
typedef struct RoomBeamSparkParams {
    s16 x, y, z, reserved06;
    s32 duration;                 /* 0x08 */
    s32 interval;                 /* 0x0C */
} RoomBeamSparkParams;

/* Sprite tpage index table: one entry per sprite kind, followed by the
 * D_800E1204 palette table. An entry read as an array element is an
 * in-struct reference, so it stays ordered after a store through the
 * parameter block base register, as retail's reads do. */
extern u16 D_800E11E4[];

#endif
