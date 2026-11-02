#ifndef PE1_ROOM_SPARK_H
#define PE1_ROOM_SPARK_H

#include "common.h"
#include "pe1/akao_send.h"
#include "pe1/asset_find.h"
#include "pe1/field_collision.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/scene_archive.h"

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

/* Particle pools reached through the two effect channels. */
typedef struct RoomSparkChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomSparkChannel;

extern struct PmPrimarySlot *g_PmSlotTable; /* running process slot (pm/process.c) */
extern int rand(void);

/* Heading helpers and the trail history: turn the angles towards a target,
 * record a position into a history of points and draw the history as a
 * ribbon. */
/* Rotates an offset by a bone's matrix. */
/* Fades a colour layer record in or out. */
extern void FieldFx_LoadRotatedClutRow(int kind, int value, void *layer);

/* Asset library lookup and the model draw calls of the model effects. */
void FieldFx_ModelSetTexture(int tpage, int clut);

/* Jitter spark: the damped spark layout with a spin angle and a size. */
typedef struct RoomJitterSpark {
    s16 x, y, z;                  /* 0x00 */
    u16 angle;                    /* 0x06 */
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 size;                     /* 0x0E */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomJitterSpark;

PE1_STATIC_ASSERT(sizeof(RoomJitterSpark) == 0x14, room_jitter_spark_size);

/* Scene object behind the primary channel: its first word points at the
 * animation state whose fifth word is the current frame. */
typedef struct RoomSparkOwnerState {
    s32 reserved[4];
    s32 frame;                    /* 0x10 */
} RoomSparkOwnerState;

typedef struct RoomSparkOwner {
    RoomSparkOwnerState *state;
} RoomSparkOwner;

/* Ring spawner parameters: centre and the ring radius. */
typedef struct RoomSparkRingParams {
    s16 x, y, z, reserved06;
    s32 radius;                   /* 0x08 */
} RoomSparkRingParams;

/* Phased spark: a damped spark with a sub-phase word. */
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

PE1_STATIC_ASSERT(sizeof(RoomPhasedSpark) == 0x18, room_phased_spark_size);

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
    u8 reserved0E[4];
    u16 trailSlot;                /* 0x12: history slot of the next trail */
} RoomSparkEventState;

/* First node of the primary channel's pool: its state byte. */
typedef struct RoomSparkNode {
    u8 reserved[0x18];
    u8 *state;
} RoomSparkNode;

/* The active battle entity: a spark that reaches it flags its actor. */
typedef struct RoomSparkActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomSparkActor;

/* 16.16 world coordinate; the integer half is read as a bitfield. */
typedef union RoomSparkCoord {
    s32 word;
    struct {
        int fraction : 16;
        int integer : 16;
    } part;
} RoomSparkCoord;

typedef struct RoomSparkBattleEntity {
    RoomSparkActor *actor;        /* 0x00 */
    u8 pad04[0x24];
    RoomSparkCoord x;             /* 0x28 */
    RoomSparkCoord y;             /* 0x2C */
    RoomSparkCoord z;             /* 0x30 */
} RoomSparkBattleEntity;

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
/* Walkable floor polygons and their count, for Geo_PointInPoly. */

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
 * g_FieldFxPaletteTable palette table. An entry read as an array element is an
 * in-struct reference, so it stays ordered after a store through the
 * parameter block base register, as retail's reads do. */

#endif
