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

extern GteRotation D_8018EFF4;
extern RenderColor D_8018EFFC;

#endif
