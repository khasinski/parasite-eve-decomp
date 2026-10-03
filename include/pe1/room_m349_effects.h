#ifndef PE1_ROOM_M349_EFFECTS_H
#define PE1_ROOM_M349_EFFECTS_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_spark.h"

/* Room m349 flare spark: jitters its velocity (states 0 and 1) or its
 * position (state 2) while it falls and bounces off the floor height. */
typedef struct RoomM349FlareSpark {
    s16 x, y, z;                  /* 0x00 */
    u16 angle;                    /* 0x06 */
    s16 vx, vy, vz;               /* 0x08 */
    s16 size;                     /* 0x0E */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomM349FlareSpark;

PE1_STATIC_ASSERT(sizeof(RoomM349FlareSpark) == 0x14, room_m349_flare_spark_size);

/* Controller target: the burst origin plus the looping sound handle. */
typedef struct RoomM349FlareTarget {
    GteShortVector position;
    s16 sound;                    /* 0x08: -1 when no sound plays */
} RoomM349FlareTarget;

typedef struct RoomM349FlareParams {
    s16 x, y, z, pad;
    s32 radius;                   /* 0x08 */
    s32 period;                   /* 0x0C: frames between ring sparks */
    s32 lastFrame;                /* 0x10: last frame that spawns ring sparks */
} RoomM349FlareParams;

extern GteRotation D_8018EFF4;
extern GteRotation D_8018F000;
extern GteRotation D_8018F008;
extern u16 D_800E11E8;
extern void func_800866A4(int handle, int arg);
extern int func_8018F010(int mode, RoomM349FlareSpark *spark);
extern RenderColor D_8018EFFC;

#endif
