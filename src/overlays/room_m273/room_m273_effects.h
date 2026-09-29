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

/* Emitter interpretation of the shared 12-byte pool record. */
typedef struct RoomM273PulseEmitterView {
    u16 x;
    u16 y;
    u16 z;
    u16 size;
    u16 phase;
    u16 unknown10;
} RoomM273PulseEmitterView;

/* Position payload shared by the player-following effect callbacks. */
typedef struct RoomM273WorldPosition {
    s32 x;
    s32 y;
    s32 z;
} RoomM273WorldPosition;

typedef struct RoomM273PlayerTransform {
    u8 reserved_00[0x14];
    RoomM273WorldPosition position;
} RoomM273PlayerTransform;

typedef struct RoomM273PlayerActorView {
    u8 reserved_00[0x0E];
    u8 kind;
    u8 reserved_0F[0x229];
    RoomM273PlayerTransform *transform;
} RoomM273PlayerActorView;

typedef struct RoomM273SpritePoolEffect {
    RoomM273WorldPosition *position;
} RoomM273SpritePoolEffect;

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
extern int D_800E27EC;
extern u16 D_800F336C, D_800E1204[];
extern int D_800F3428;
extern volatile u16 D_800F3376, D_800F3378;

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
PE1_STATIC_ASSERT(sizeof(RoomM273PulseEmitterView) == 0x0C,
                  room_m273_pulse_emitter_view_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseEmitterView, x) == 0,
                  room_m273_pulse_emitter_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseEmitterView, y) == 2,
                  room_m273_pulse_emitter_y_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseEmitterView, z) == 4,
                  room_m273_pulse_emitter_z_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseEmitterView, size) == 6,
                  room_m273_pulse_emitter_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseEmitterView, phase) == 8,
                  room_m273_pulse_emitter_phase_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseEmitterView, unknown10) == 0x0A,
                  room_m273_pulse_emitter_unknown_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273WorldPosition) == 0x0C,
                  room_m273_world_position_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PlayerTransform, position) == 0x14,
                  room_m273_player_transform_position_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273PlayerTransform) == 0x20,
                  room_m273_player_transform_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PlayerActorView, kind) == 0x0E,
                  room_m273_player_actor_kind_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PlayerActorView, transform) == 0x238,
                  room_m273_player_actor_transform_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273PlayerActorView) == 0x23C,
                  room_m273_player_actor_view_size);
PE1_STATIC_ASSERT(sizeof(RoomM273SpritePoolEffect) == 4,
                  room_m273_sprite_pool_effect_size);
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
