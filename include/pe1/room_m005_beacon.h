#ifndef PE1_ROOM_M005_BEACON_H
#define PE1_ROOM_M005_BEACON_H

#include "common.h"
#include "pe1/asset_find.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_transform_point.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/render_prim.h"
#include "pe1/room_floor.h"
#include "pe1/room_m005_drifting_sprite.h"
#include "pe1/scene_archive.h"

/* room_m005 joint beacon (RoomM005_JointBeacon): follows joint 0x14 of the actor,
 * sheds drifting sprites, swells and shrinks, tags the player on contact and
 * draws four passes of a model, three glow sprites and a turning second
 * model while the effect starts. */
typedef struct RoomM005Beacon {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector rotation;
    /* 0x18 */ int height;
    /* 0x1C */ int width;
    /* 0x20 */ s16 phase;
    /* 0x22 */ s16 timer;
} RoomM005Beacon;

typedef struct RoomM005BeaconSprite {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ s16 pad06;
    /* 0x08 */ s16 vx, vy, vz;
} RoomM005BeaconSprite;

typedef struct RoomM005BeaconSpawnChannel {
    s32 reserved[2];
    void *pool;
} RoomM005BeaconSpawnChannel;

typedef struct RoomM005BeaconNode {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0x14];
    /* 0x18 */ u8 *state;
} RoomM005BeaconNode;

typedef struct RoomM005BeaconPool {
    /* 0x000 */ RoomM005BeaconNode *node;
    /* 0x004 */ u8 reserved004[0x234];
    /* 0x238 */ GteMatrix *matrix;
} RoomM005BeaconPool;

typedef struct RoomM005BeaconChannel {
    s32 reserved[2];
    RoomM005BeaconPool *pool;
} RoomM005BeaconChannel;

typedef struct RoomM005BeaconActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM005BeaconActor;

typedef struct RoomM005BeaconEntity {
    RoomM005BeaconActor *actor;
} RoomM005BeaconEntity;

typedef struct RoomM005BeaconEvent {
    /* 0x00 */ u8 reserved[0xD];
    /* 0x0D */ u8 active;
    /* 0x0E */ u8 reserved0E[4];
    /* 0x12 */ s16 variant;
} RoomM005BeaconEvent;

typedef struct RoomM005BeaconOffset {
    s32 word[2];
} __attribute__((packed)) RoomM005BeaconOffset;

typedef struct RoomM005BeaconColor {
    u8 r, g, b, code;
} RoomM005BeaconColor;

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern RoomM005BeaconOffset g_RoomM005JointBeaconAnchor;
extern u8 *g_RoomM005BeaconModel0;
extern u8 *g_RoomM005BeaconModel1;
extern GteShortVector g_RoomM005BeaconOffset;
extern GteShortVector g_RoomM005BeaconCenter;

void FieldFx_ModelShiftColors(u8 *data, int r, int g, int b);
int FieldFx_IsPlayerInSegmentBand(void *position, GteShortVector *target, int radius);
int rand(void);

#endif
