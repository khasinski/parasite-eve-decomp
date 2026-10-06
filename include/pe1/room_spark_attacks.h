#ifndef PE1_ROOM_SPARK_ATTACKS_H
#define PE1_ROOM_SPARK_ATTACKS_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_spark.h"

/* The spark attacks linked by room_m156+2, room_m291+2 and room_m380+2
 * (src/overlays/room_lib/RoomEffect_SparkAttacks.c): burst orbs launched
 * from the actor's hand, a spray of damped sparks, a beam with a bouncing
 * spark shower and a ring of jittering sparks. */

/* Eight-byte joint offset copied to the stack unaligned, as retail does. */
typedef struct RoomSparkWords8 {
    s32 word[2];
} __attribute__((packed)) RoomSparkWords8;

/* Per-room data: the heading the burst orbs fly along, set by the orb
 * controller from the actor's rotation, and the beam's model asset. */
extern s16 g_RoomBurstOrbHeading;
extern void *g_RoomBeamSparkAsset;

#endif
