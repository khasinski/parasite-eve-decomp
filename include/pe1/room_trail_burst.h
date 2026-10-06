#ifndef PE1_ROOM_TRAIL_BURST_H
#define PE1_ROOM_TRAIL_BURST_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_spark.h"

/* The trail and burst attacks linked by room_m245 and room_m397
 * (src/overlays/room_lib/RoomEffect_TrailBurstAttacks.c): two seeking
 * trails that chase the player and shed sparks and flashes, each with its
 * controller, and two model bursts that grow a scaled model at one of the
 * actor's joints while shedding pulsing sprites. */

/* Head of a seeking trail and the sparks and flashes it spawns; all three
 * share this record and are told apart by their state. */
typedef struct RoomSeekingTrail {
    s16 x, y, z;                  /* 0x00 */
    s16 slot;                     /* 0x06: trail history index */
    s16 ax, ay, az;               /* 0x08: heading angles */
    s16 fade;                     /* 0x0E */
    s16 speed;                    /* 0x10 */
    u16 turn;                     /* 0x12 */
    s16 state;                    /* 0x14 */
    s16 timer;                    /* 0x16 */
} RoomSeekingTrail;

PE1_STATIC_ASSERT(sizeof(RoomSeekingTrail) == 0x18, room_seeking_trail_size);

/* The last 25 points of one trail. */
typedef struct RoomSeekingTrailHistory {
    GteShortVector point[25];
} RoomSeekingTrailHistory;

/* Sprite the model burst sheds: it drifts by its velocity and pulses with
 * a cosine of its frame counter. */
typedef struct RoomModelBurstParticle {
    u16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s16 reserved08;
    u16 frame;                    /* 0x0A */
    u16 vx;                       /* 0x0C */
    u16 vy;                       /* 0x0E */
} RoomModelBurstParticle;

PE1_STATIC_ASSERT(sizeof(RoomModelBurstParticle) == 0x10,
                  room_model_burst_particle_size);

/* Per-room data: the trail histories (two trails each), the colour ramps,
 * the model assets, the layer records and the anchors of the two bursts. */
extern RoomSeekingTrailHistory g_RoomSeekingTrailHistory[];
extern RoomSeekingTrailHistory g_RoomScatterTrailHistory[];
extern u8 g_RoomModelBurstColors[];
extern u8 g_RoomScatterBurstColors[];
extern void *g_RoomModelBurstAsset;
extern void *g_RoomScatterBurstAsset;
extern u8 g_RoomModelBurstLayer[];
extern u8 g_RoomScatterBurstLayer[];
/* Where each burst last drew its model; its shed sprites are drawn there. */
extern GteShortVector g_RoomModelBurstAnchor;
extern GteShortVector g_RoomScatterBurstAnchor;

#endif
