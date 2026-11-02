#ifndef PE1_ROOM_SHAKE_BURST_H
#define PE1_ROOM_SHAKE_BURST_H

#include "common.h"
#include "pe1/akao_send.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/room_fx.h"
#include "pe1/room_orbit_trail.h"
#include "pe1/scene_archive.h"

/* Shake burst controller (room_m318): shakes the camera while it scatters
 * debris particles around its anchor, then flags the actor for the
 * scene script. */

typedef struct RoomShakeBurstPoint {
    s16 x, y, z;
} RoomShakeBurstPoint;

typedef struct RoomShakeBurstParticleCallbackView {
    u16 x, y, z;                  /* 0x00 */
    u16 reserved06;               /* 0x06: pool stride is eight bytes */
} RoomShakeBurstParticleCallbackView;

PE1_STATIC_ASSERT(sizeof(RoomShakeBurstParticleCallbackView) == 8,
                  room_shake_burst_particle_size);

typedef struct RoomShakeBurstObject {
    u32 flags;                    /* 0x00 */
    u8 reserved04[0x14];
    u8 *status;                   /* 0x18 */
} RoomShakeBurstObject;

typedef struct RoomShakeBurstSlot {
    RoomShakeBurstObject *object;
} RoomShakeBurstSlot;

typedef struct RoomShakeBurstChannel {
    s32 reserved[2];
    RoomShakeBurstSlot *pool;     /* 0x08 */
} RoomShakeBurstChannel;

typedef RoomOrbitTrailPoolChannel RoomShakeBurstSparkChannel;

typedef struct RoomShakeBurstEntity {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomShakeBurstEntity;

typedef struct RoomShakeBurstEvent {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
    u8 reserved0E[8];
    s16 running;                  /* 0x16 */
} RoomShakeBurstEvent;

/* Sweep shake controller anchor: position plus the sweep angle handed to
 * each spawned particle. */
typedef struct RoomShakeSweepAnchor {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s32 angle;                    /* 0x08 */
} RoomShakeSweepAnchor;

/* Sweep particle: a scaled model that turns about its own axis while it
 * follows the sweep centre (state 0) or decays in place (state 1). */
typedef struct RoomShakeSweepParticle {
    s16 x, y, z;                  /* 0x00 */
    s16 state;                    /* 0x06 */
    s16 size;                     /* 0x08 */
    u16 angle;                    /* 0x0A */
    u16 timer;                    /* 0x0C */
    s16 stretch;                  /* 0x0E */
} RoomShakeSweepParticle;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomShakeSweepParticle, state) == 0x06,
                  room_shake_sweep_state_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomShakeSweepParticle, stretch) == 0x0E,
                  room_shake_sweep_stretch_offset);
PE1_STATIC_ASSERT(sizeof(RoomShakeSweepParticle) == 0x10,
                  room_shake_sweep_particle_size);

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;

extern int rand(void);
extern void Battle_HaltOnPositiveX(void);
extern u8 *D_800F32D8;
extern u8 *D_800F2D98;
extern void Battle_InitEntityColors(void);

/* room_m318 particle callbacks and the sweep anchor. */
extern int RoomM318_ShakeBurstParticleCallback(int mode,
                         RoomShakeBurstParticleCallbackView *particle);
extern int RoomM318_ShakeSweepParticleCallback(int mode, RoomShakeSweepParticle *particle);
extern GteShortVector g_RoomM318ShakeSweepAnchor;

/* Sweep particle draw: model asset, matrix helpers and clut lookups. */
extern void FieldFx_ModelSetTexture(int tpage, int clut);

/* Flash burst controller (RoomM318_FlashBurst): its particle callback, colour
 * seed, glow track and the helpers it calls. */
extern int RoomM318_FlashBurstParticleCallback(int mode, RoomOrbitTrailParticle *p);
extern RenderColor g_RoomM318FlashBurstColor;
extern u8 g_RoomM318FlashBurstColorTrack[];

/* Rising column controller (RoomM318_RisingColumn): the anchor rises from the
 * floor over the configured duration while sparks fall off it. */
typedef struct RoomRisingColumn {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s16 counter;                  /* 0x08 */
    s16 timer;                    /* 0x0A */
} RoomRisingColumn;

typedef struct RoomRisingColumnParams {
    s32 descend;                  /* 0x00 */
    s32 duration;                 /* 0x04 */
    s32 height;                   /* 0x08 */
    s32 offset;                   /* 0x0C */
} RoomRisingColumnParams;

extern GteShortVector g_RoomM318RisingColumnAnchor;
extern int RoomM318_RisingColumnParticleCallback(int mode, RoomOrbitTrailParticle *p);
extern GteRotation g_RoomM318FlatTilt;
extern u8 g_RoomM318RisingColumnParticleColorTrack[];
extern u16 g_FieldFxTpageSlot10;

/* Cross flash controller (RoomM318_CrossFlash): an anchor with an orientation,
 * and particles that carry their own position, rotation and draw kind. */
typedef struct RoomCrossFlash {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    GteRotation rotation;         /* 0x08 */
} RoomCrossFlash;

typedef struct RoomCrossFlashParticle {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    GteRotation rotation;         /* 0x08: flags selects the flash draw */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomCrossFlashParticle;

extern GteShortVector g_RoomM318CrossFlashAnchor;
extern int RoomM318_CrossFlashParticleCallback(int mode, RoomCrossFlashParticle *p);
extern RenderColor g_RoomM318EmberParticleColor;
extern u8 g_RoomM318CrossFlashParticleColorTrack[];

/* Ember burst particle (RoomM318_EmberBurstParticleCallback): spin template, colour seed and
 * glow track. */
extern GteRotation g_RoomM318ZeroSpin;
extern u8 g_RoomM318EmberBurstParticleColorTrack[];

#endif
