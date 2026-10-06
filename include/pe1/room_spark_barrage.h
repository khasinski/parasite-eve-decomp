#ifndef PE1_ROOM_SPARK_BARRAGE_H
#define PE1_ROOM_SPARK_BARRAGE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_spark.h"

/* The spark barrage linked by room_m188 and room_m390
 * (src/overlays/room_lib/RoomEffect_SparkBarrage.c): five spark attacks,
 * each a particle callback and its controller. Phased sparks drop from
 * above the player and flash the scene, bouncing sparks spray from the
 * actor's hand, wave sparks ride the actor's joints, comet sparks are fired
 * along random directions and lifted sparks rise from the actor. */

/* Trail record the comet draw hands to the trail renderer: velocity,
 * drift, steer and position in that order. */
typedef struct RoomCometSparkTrail {
    GteRotation velocity;
    GteRotation drift;
    GteRotation steer;
    GteRotation position;
} RoomCometSparkTrail;

/* Per-room data: the colour ramps of the bouncing, wave, comet and lifted
 * sparks and the comet's trail record. */
extern u8 g_RoomBouncingSparkColors[];
extern u8 g_RoomWaveSparkColors[];
extern u8 g_RoomCometSparkColors[];
extern RoomCometSparkTrail g_RoomCometSparkTrail;
extern u8 g_RoomLiftedSparkColors[];

#endif
