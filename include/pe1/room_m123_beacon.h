#ifndef PE1_ROOM_M123_BEACON_H
#define PE1_ROOM_M123_BEACON_H

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
#include "pe1/scene_archive.h"

/* room_m123 joint beacon (RoomM123_JointBeacon): follows joint 0x17 of the actor,
 * swells, pulses and fades, tags the player once on contact, and draws a glow
 * sprite pair with star fans, a fading ground ring of three sprites, a
 * turning model while the effect starts and three passes of a second model. */
typedef struct RoomM123Beacon {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector rotation;
    /* 0x18 */ int height;
    /* 0x1C */ int width;
    /* 0x20 */ int depth;
    /* 0x24 */ s16 alpha;
    /* 0x26 */ s16 fade;
    /* 0x28 */ s16 armed;
    /* 0x2A */ s16 phase;
    /* 0x2C */ s16 timer;
    /* 0x2E */ s16 glow;
    /* 0x30 */ s16 shade;
} RoomM123Beacon;

typedef struct RoomM123BeaconNode {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0x14];
    /* 0x18 */ u8 *state;
} RoomM123BeaconNode;

typedef struct RoomM123BeaconPool {
    /* 0x000 */ RoomM123BeaconNode *node;
    /* 0x004 */ u8 reserved004[0x234];
    /* 0x238 */ GteMatrix *matrix;
} RoomM123BeaconPool;

typedef struct RoomM123BeaconChannel {
    s32 reserved[2];
    RoomM123BeaconPool *pool;
} RoomM123BeaconChannel;

typedef struct RoomM123BeaconActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM123BeaconActor;

typedef struct RoomM123BeaconEntity {
    RoomM123BeaconActor *actor;
} RoomM123BeaconEntity;

typedef struct RoomM123BeaconEvent {
    /* 0x00 */ u8 reserved[0xD];
    /* 0x0D */ u8 active;
    /* 0x0E */ u8 reserved0E[4];
    /* 0x12 */ s16 variant;
} RoomM123BeaconEvent;

typedef struct RoomM123BeaconOffset {
    s32 word[2];
} __attribute__((packed)) RoomM123BeaconOffset;

typedef struct RoomM123BeaconColor {
    u8 r, g, b, code;
} RoomM123BeaconColor;

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern RoomM123BeaconOffset g_RoomM123BeaconAnchor;
extern u8 *g_RoomM123BeaconModel0;
extern u8 *g_RoomM123BeaconModel1;
extern u8 g_RoomM123BeaconColorTrack[];

void FieldFx_DrawPulledSprite(GteShortVector *position, int scale_x, int scale_y, int texture,
                   int clut, int page, int intensity, int unused, int angle);
int FieldFx_IsPlayerInSegmentBand(void *position, GteShortVector *target, int radius);
GteMatrix *MulMatrix0(GteMatrix *first, GteMatrix *second, GteMatrix *out);

#endif
