#ifndef PE1_ROOM_M089_MODEL_PULSE_H
#define PE1_ROOM_M089_MODEL_PULSE_H

#include "common.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/room_m089_spin_model.h"
#include "pe1/room_swirl_rise.h"

/* room_m089 pulsing model burst (RoomM089_ModelPulse): three turning layers of
 * one model and a fourth model over a glow strip, while swirl/rise sprites
 * spawn around the anchor and the player is tagged on contact. */
typedef struct RoomM089ModelPulse {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ s16 phase;
    /* 0x0A */ s16 timer;
} RoomM089ModelPulse;

typedef struct RoomM089PulseNode {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0x14];
    /* 0x18 */ u8 *state;
} RoomM089PulseNode;

typedef struct RoomM089PulsePool {
    RoomM089PulseNode *node;
} RoomM089PulsePool;

typedef struct RoomM089PulseChannel {
    s32 reserved[2];
    RoomM089PulsePool *pool;
} RoomM089PulseChannel;

typedef struct RoomM089SpawnChannel {
    s32 reserved[2];
    void *pool;
} RoomM089SpawnChannel;

typedef struct RoomM089PulseActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM089PulseActor;

typedef struct RoomM089PulseEntity {
    RoomM089PulseActor *actor;
} RoomM089PulseEntity;

typedef struct RoomM089PulseEvent {
    u8 reserved[0xD];
    u8 active;
} RoomM089PulseEvent;

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern u8 *g_RoomM089PulseModel;
extern u8 g_RoomM089PulseColorTrack[];

int RoomM089_SwirlSpriteCallback(int mode, RoomSwirlRiseState *state);
int rand(void);

#endif
