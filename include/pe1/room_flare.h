#ifndef PE1_ROOM_FLARE_H
#define PE1_ROOM_FLARE_H

#include "common.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_transform_point.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/floor_polygon.h"
#include "pe1/gte_short_vector.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/room_floor.h"
#include "pe1/room_fx.h"
#include "pe1/room_interface.h"

/* Flare effects: a controller that orbits an anchor read from the scene
 * object and sheds drift particles, and a dropped flare particle that falls,
 * flashes the scene on impact and scatters children. */

typedef struct RoomFlareChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomFlareChannel;

typedef struct RoomFlareNode {
    u8 reserved[0x18];
    u8 *state;                    /* 0x18 */
} RoomFlareNode;

typedef struct RoomFlareEventState {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
    u8 reserved0E[4];
    s16 phase;                    /* 0x12 */
} RoomFlareEventState;

typedef struct RoomFlareActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomFlareActor;

typedef struct RoomFlareBattleEntity {
    RoomFlareActor *actor;
} RoomFlareBattleEntity;

/* First word of the packet the effect channel's pool points at. */
typedef struct RoomFlarePacket {
    u32 code;
} RoomFlarePacket;

/* Scene object reached through the primary channel pool. */
typedef struct RoomFlareAnchor {
    u8 reserved[0x268];
    s16 x, y, z;                  /* 0x268 */
} RoomFlareAnchor;

typedef struct RoomFlareColor {
    u8 r, g, b, code;
} RoomFlareColor;

/* Orbit flare controller: anchor, state, timer, sound, sweep angle. */
typedef struct RoomOrbitFlareState {
    s16 x, y, z, reserved06;
    s16 state;                    /* 0x08 */
    u16 frame;                    /* 0x0A */
    s16 soundHandle;              /* 0x0C */
    s16 angle;                    /* 0x0E */
    s16 height;                   /* 0x10 */
    s16 spread;                   /* 0x12 */
} RoomOrbitFlareState;

typedef struct RoomOrbitFlareSpawn {
    s32 radius;                   /* 0x00 */
    s32 angle;                    /* 0x04 */
    s32 attached;                 /* 0x08 */
} RoomOrbitFlareSpawn;

/* Dropped flare particle: position, swing, velocity, state, timer. */
typedef struct RoomDroppedFlare {
    s16 x, y, z;                  /* 0x00 */
    s16 swing;                    /* 0x06 */
    GteShortVector velocity;      /* 0x08 */
    s16 state;                    /* 0x10 */
    u16 timer;                    /* 0x12 */
} RoomDroppedFlare;

/* The object that drops the flares: its position and the direction the
 * flares leave in. */
typedef struct RoomDroppedFlareEmitter {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    GteShortVector angles;        /* 0x08 */
} RoomDroppedFlareEmitter;

/* Eight-byte sprite rectangle the emitter hands to its effect channel;
 * byte-typed, so it is copied with unaligned word moves. */
typedef struct RoomFlareRect {
    u8 bytes[8];
} RoomFlareRect;

typedef struct RoomDroppedFlareSpawn {
    s32 reserved00;
    s32 lift;                     /* 0x04 */
} RoomDroppedFlareSpawn;

PE1_STATIC_ASSERT(sizeof(RoomOrbitFlareState) == 0x14, room_orbit_flare_state_size);
PE1_STATIC_ASSERT(sizeof(RoomDroppedFlare) == 0x14, room_dropped_flare_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFlareAnchor, x) == 0x268, room_flare_anchor_x);

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern u16 g_FieldFxPaletteEntry2;

extern int rand(void);
extern void Akao_Cmd_21(int, int);

/* Room data the dropped-flare unit writes; each room defines it. */
extern int RoomLib_PairA;
extern int RoomLib_PairB;

int RoomEffect_DroppedFlareParticle(int mode, RoomDroppedFlare *flare,
                                    RoomDroppedFlareSpawn *spawn);

#endif
