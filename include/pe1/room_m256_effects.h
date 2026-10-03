#ifndef PE1_ROOM_M256_EFFECTS_H
#define PE1_ROOM_M256_EFFECTS_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_spark.h"

/* Room m256 spark that jitters (state 0) or swirls on a spinning circle
 * (state 1) while it falls and bounces off the floor height. */
typedef struct RoomM256SwirlSpark {
    s16 x, y, z;                  /* 0x00 */
    s16 small;                    /* 0x06: nonzero draws with page 1 */
    s16 vx, vy, vz;               /* 0x08 */
    s16 spin;                     /* 0x0E */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomM256SwirlSpark;

PE1_STATIC_ASSERT(sizeof(RoomM256SwirlSpark) == 0x14, room_m256_swirl_spark_size);

#endif
