#ifndef PE1_ROOM_M089_MODEL_PULSE_H
#define PE1_ROOM_M089_MODEL_PULSE_H

#include "common.h"
#include "pe1/room_m089_spin_model.h"
#include "pe1/room_swirl_rise.h"

/* room_m089 pulsing model burst (func_80193618): three turning layers of
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

extern RoomM089PulseChannel *D_800F32D0;
extern RoomM089SpawnChannel *D_800F33E0;
extern RoomM089PulseEvent *D_800E2368;
extern RoomM089PulseEntity *D_8009D254;
extern u8 *D_8019416C;
extern u8 D_801940F8[];
extern u16 D_800E11EA;

int func_80193200(int mode, RoomSwirlRiseState *state);
RoomSwirlRiseState *func_800CE610(void *pool);
int func_800CE560(void *pool, int count, int size, void *callback);
int func_800C6B90(void *position, int radius);
int func_80071A54(void);

#endif
