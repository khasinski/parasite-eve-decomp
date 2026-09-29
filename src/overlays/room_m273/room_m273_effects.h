#ifndef ROOM_M273_EFFECTS_H
#define ROOM_M273_EFFECTS_H

#include "common.h"

/* The third word of the shared allocator context points at its pool. */
typedef struct RoomM273PoolContext {
    s32 reserved[2];
    void *pool;
} RoomM273PoolContext;

/* Common prefix read by the threshold-effect controller family. */
typedef struct RoomM273EffectModeState {
    u8 reserved_00[0x0E];
    u8 kind;
    u8 reserved_0F[7];
    s16 value22;
    u8 reserved_18[2];
    u16 value26;
} RoomM273EffectModeState;

typedef struct RoomM273PulseSeed {
    u8 reserved_00[2];
    u16 angle;
    u8 reserved_06[4];
} RoomM273PulseSeed;

typedef struct RoomM273ThresholdTransform {
    u8 reserved_00[0x594];
    s32 x;
    s32 y;
    s32 z;
} RoomM273ThresholdTransform;

typedef struct RoomM273ThresholdEntityState {
    RoomM273EffectModeState mode;
    u8 reserved_1C[0x21C];
    RoomM273ThresholdTransform *transform;
} RoomM273ThresholdEntityState;

/* D_800F32D0 points at either view as each controller is active. */
typedef union RoomM273EffectStatePointer {
    RoomM273EffectModeState *mode;
    RoomM273ThresholdEntityState *threshold;
} RoomM273EffectStatePointer;

typedef struct RoomM273EffectStateContext {
    u8 reserved_00[8];
    RoomM273EffectStatePointer state;
} RoomM273EffectStateContext;

extern RoomM273PoolContext *D_800F33E0;

PE1_STATIC_ASSERT(sizeof(RoomM273PoolContext) == 0x0C,
                  room_m273_pool_context_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PoolContext, pool) == 8,
                  room_m273_pool_context_pool_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273EffectModeState, kind) == 0x0E,
                  room_m273_effect_state_kind_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273EffectModeState, value22) == 0x16,
                  room_m273_effect_state_value22_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273EffectModeState, value26) == 0x1A,
                  room_m273_effect_state_value26_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273EffectModeState) == 0x1C,
                  room_m273_effect_state_size);
PE1_STATIC_ASSERT(sizeof(RoomM273PulseSeed) == 8, room_m273_pulse_seed_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273ThresholdTransform, x) == 0x594,
                  room_m273_threshold_transform_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273ThresholdEntityState, transform) == 0x238,
                  room_m273_threshold_entity_transform_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273ThresholdEntityState) == 0x23C,
                  room_m273_threshold_entity_state_size);
PE1_STATIC_ASSERT(sizeof(RoomM273ThresholdTransform) == 0x5A0,
                  room_m273_threshold_transform_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273EffectStateContext, state) == 8,
                  room_m273_effect_context_state_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273EffectStatePointer, mode) == 0,
                  room_m273_effect_state_mode_pointer_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273EffectStatePointer, threshold) == 0,
                  room_m273_effect_state_threshold_pointer_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273EffectStatePointer) == 4,
                  room_m273_effect_state_pointer_size);
PE1_STATIC_ASSERT(sizeof(RoomM273EffectStateContext) == 0x0C,
                  room_m273_effect_context_size);

#endif
