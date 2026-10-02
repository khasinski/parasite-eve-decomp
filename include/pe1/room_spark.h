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

#endif
