#ifndef ROOM_M273_EFFECTS_H
#define ROOM_M273_EFFECTS_H

#include "common.h"
#include "pe1/gte_types.h"

/* Shared 8-byte particle payload used by the rising-sprite callbacks. */
typedef struct RoomM273RisingParticle {
    s16 x;
    u16 y;
    s16 z;
    u16 speed;
} RoomM273RisingParticle;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingParticle, y) == 2,
                  room_m273_rising_particle_y_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingParticle, z) == 4,
                  room_m273_rising_particle_z_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingParticle, speed) == 6,
                  room_m273_rising_particle_speed_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273RisingParticle) == 8,
                  room_m273_rising_particle_size);

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

typedef struct RoomM273PairEmitterFields {
    u16 x;
    u16 y;
    u16 z;
    u16 flags;
} RoomM273PairEmitterFields;

/* The same eight-byte pool entry is also copied as four opaque halfwords. */
typedef union RoomM273PairPoolRecord {
    RoomM273PairEmitterFields emitter;
    u16 values[4];
} RoomM273PairPoolRecord;

/* Controller work buffers use parallel coordinate arrays and a shared count. */
typedef struct RoomM273PairedPulseBatch {
    u16 x[8];
    u16 y[8];
    u16 z[8];
    s16 count;
    u8 reserved_32[0x0E];
    u8 stopped;
} RoomM273PairedPulseBatch;

typedef struct RoomM273RisingBatchBuffer {
    u16 x[12];
    u16 y[12];
    u16 z[12];
    s16 count;
    u8 reserved_4A[0x13];
    u8 stopped;
} RoomM273RisingBatchBuffer;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PairedPulseBatch, count) == 0x30,
                  room_m273_paired_pulse_count_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PairedPulseBatch, stopped) == 0x40,
                  room_m273_paired_pulse_stopped_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273PairedPulseBatch) == 0x42,
                  room_m273_paired_pulse_batch_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingBatchBuffer, count) == 0x48,
                  room_m273_rising_batch_count_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273RisingBatchBuffer, stopped) == 0x5D,
                  room_m273_rising_batch_stopped_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273RisingBatchBuffer) == 0x5E,
                  room_m273_rising_batch_buffer_size);

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

/* Several sprite pools store only a pointer to their position payload. */
typedef struct RoomM273PointerPoolEffect {
    void *position;
} RoomM273PointerPoolEffect;

typedef RoomM273PointerPoolEffect RoomM273SpritePoolEffect;

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

typedef struct RoomM273DirectedRingOwner {
    u8 reserved_00[8];
    void *value;
} RoomM273DirectedRingOwner;

typedef struct RoomM273DirectedRingState {
    RoomM273DirectedRingOwner *owner;
} RoomM273DirectedRingState;

typedef struct RoomM273ScaledLayerOwner {
    u8 reserved_00[0x238];
    RoomM273ThresholdTransform *transform;
} RoomM273ScaledLayerOwner;

/* D_800F32D0 points at either view as each controller is active. */
typedef union RoomM273EffectStatePointer {
    RoomM273EffectModeState *mode;
    RoomM273ThresholdEntityState *threshold;
    RoomM273DirectedRingState *directed_rings;
    RoomM273ScaledLayerOwner *scaled_layer;
} RoomM273EffectStatePointer;

typedef struct RoomM273EffectStateContext {
    u8 reserved_00[8];
    RoomM273EffectStatePointer state;
} RoomM273EffectStateContext;

extern RoomM273PoolContext *D_800F33E0;
extern int D_800E27EC;
extern u16 D_800F336C, D_800E1204[];
extern int D_800F3428;
extern u16 D_800F3376, D_800F3378;

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
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PairEmitterFields, flags) == 6,
                  room_m273_pair_emitter_flags_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273PairPoolRecord) == 8,
                  room_m273_pair_pool_record_size);
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
PE1_STATIC_ASSERT(sizeof(RoomM273PointerPoolEffect) == 4,
                  room_m273_pointer_pool_effect_size);
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
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273DirectedRingOwner, value) == 8,
                  room_m273_directed_ring_owner_value_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273DirectedRingOwner) == 0x0C,
                  room_m273_directed_ring_owner_size);
PE1_STATIC_ASSERT(sizeof(RoomM273DirectedRingState) == 4,
                  room_m273_directed_ring_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273ScaledLayerOwner, transform) == 0x238,
                  room_m273_scaled_layer_transform_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273ScaledLayerOwner) == 0x23C,
                  room_m273_scaled_layer_owner_size);
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
