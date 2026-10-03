#ifndef ROOM_M273_H
#define ROOM_M273_H

#include "pe1/render_object.h"
#include "room_m273_effects.h"

typedef GteShortVector RoomM273Vector;

typedef struct {
    signed int low : 16;
    signed int high : 16;
} RoomM273TrigEntry;

typedef struct RoomM273PaletteWord {
    u8 bytes[4];
} RoomM273PaletteWord;

typedef struct RoomM273PaletteEffect {
    RoomM273PaletteWord color;
    GteVector *source;
    s16 size;
    s16 depth;
    s16 x;
    s16 y;
} RoomM273PaletteEffect;

typedef struct RoomM273PaletteInput {
    GteVector *source;
    s16 size;
} RoomM273PaletteInput;

typedef struct RoomM273PulseInput {
    GteShortVector position;
    u16 velocity;
    u16 unknown_0A;
} RoomM273PulseInput;

typedef union RoomM273PulseRecord {
    RoomM273PulseInput callback;
    RoomM273PulseEmitterView emitter;
} RoomM273PulseRecord;

typedef struct RoomM273TemplatePoint {
    u16 x;
    u16 y;
    u16 z;
    u16 flags;
} RoomM273TemplatePoint;

typedef union RoomM273DelayedBurstRecord {
    RoomM273TemplatePoint emitter;
    RoomM273RisingParticle callback;
} RoomM273DelayedBurstRecord;

typedef struct RoomM273SampledLayerEffect {
    GteShortVector position;
    u8 parameter[4];
} RoomM273SampledLayerEffect;

PE1_STATIC_ASSERT(sizeof(RoomM273PaletteWord) == 4, room_m273_palette_word_size);
PE1_STATIC_ASSERT(sizeof(RoomM273PaletteEffect) == 16, room_m273_palette_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PaletteInput, size) == 4,
                  room_m273_palette_input_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseInput, velocity) == 8,
                  room_m273_pulse_velocity_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseInput, unknown_0A) == 0x0A,
                  room_m273_pulse_unknown_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273PulseInput) == 0x0C,
                  room_m273_pulse_input_size);
PE1_STATIC_ASSERT(sizeof(RoomM273PulseRecord) == 0x0C,
                  room_m273_pulse_record_size);
PE1_STATIC_ASSERT(sizeof(RoomM273TemplatePoint) == 8,
                  room_m273_template_point_size);
PE1_STATIC_ASSERT(sizeof(RoomM273DelayedBurstRecord) == 8,
                  room_m273_delayed_burst_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273TemplatePoint, flags) ==
                      PE1_OFFSETOF(RoomM273RisingParticle, speed),
                  room_m273_delayed_burst_tail_offset);

extern RoomM273TrigEntry D_800966EC[];
extern s16 D_8019AE98;
extern int D_8019ABFC[];
extern s16 D_800F336A;
extern s16 D_8019ACC0[];
extern u8 D_8019AFA2;
extern u8 D_8019AF8A[];
extern u16 D_800E11E8;
extern volatile u16 D_800F3376;
extern volatile u16 D_800F3378;
extern void *D_8019AE94;
extern RoomM273PaletteWord D_8019AC30[];
extern u16 D_800942EC;
extern char D_8019AB70[];
extern char D_8019ACC8[];
extern char D_8019ACCC[];
void *func_800CE610(void *pool);
int Inv_ScrambleGrid(void);
int func_80199568(int mode, RoomM273RisingParticle *particle);
int func_800CE560(void *pool, int size, int count, int (*callback)());

typedef struct RoomPlacementMap {
    u8 pad_000[0x594];
    s32 x;
    s32 y;
    s32 z;
} RoomPlacementMap;

typedef struct RoomPlacementOwner {
    u8 pad_000[0x238];
    RoomPlacementMap *map;
} RoomPlacementOwner;

typedef struct RoomPlacementStateContext {
    u8 reserved[8];
    RoomPlacementOwner *owner;
} RoomPlacementStateContext;

typedef struct RoomPlacementState {
    u8 pad_00[4];
    int *signal;
    s16 phase_countdown;
    u8 pad_0A[0x26];
    s32 vertical_step;
    s32 fallback_x;
    s32 fallback_z;
    s16 vertical_ticks;
    u8 placement_checked;
    u8 active;
    u8 pending_action;
} RoomPlacementState;

typedef struct RoomSelectionState RoomSelectionState;
struct RoomSelectionState {
    u8 pad_00[8];
    struct FieldActor *actor;
    void (*callback)(RoomSelectionState *);
    int *signal;
    u8 pad_14[8];
    RenderMatrix matrix;
    u8 pad_3c[4];
    int savedX;
    int savedZ;
    u8 pad_48[3];
    u8 activated;
};

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, callback) == 0x0C,
                  room_selection_callback_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, signal) == 0x10,
                  room_selection_signal_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, actor) == 0x08,
                  room_selection_actor_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, matrix) == 0x1c,
                  room_selection_matrix_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, activated) == 0x4b,
                  room_selection_activated_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, callback) +
                      PE1_OFFSETOF(RoomPlacementState, active) ==
                      PE1_OFFSETOF(RoomSelectionState, activated),
                  room_selection_placement_state_overlap);

void func_80192664(RoomSelectionState *state);
s32 func_80192D8C(RoomSelectionState *selection);

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementOwner, map) == 0x238,
                  room_placement_map_offset);
PE1_STATIC_ASSERT(sizeof(RoomM273SampledLayerEffect) == 12,
                  room_m273_sampled_layer_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273SampledLayerEffect, parameter) == 8,
                  room_m273_sampled_layer_effect_parameter_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementStateContext, owner) == 8,
                  room_placement_state_context_owner_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementMap, x) == 0x594,
                  room_placement_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementState, active) == 0x3F,
                  room_placement_active_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementState, signal) == 4,
                  room_placement_signal_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementState, phase_countdown) == 8,
                  room_placement_phase_countdown_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementState, vertical_step) == 0x30,
                  room_placement_vertical_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementState, vertical_ticks) == 0x3C,
                  room_placement_vertical_ticks_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementState, placement_checked) == 0x3E,
                  room_placement_checked_offset);

void func_80192C00(RoomPlacementOwner *owner, RoomPlacementState *state);

typedef struct RoomOscillationEffect {
    u8 pad_00[8];
    struct FieldActor *actor;
    int *angleOut;
    int *distanceOut;
    u8 pad_14[4];
    int frame;
    int baseHeight;
    s16 active;
    s16 amplitude;
    s16 period;
} RoomOscillationEffect;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOscillationEffect, actor) == 0x08,
                  room_oscillation_actor_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOscillationEffect, frame) == 0x18,
                  room_oscillation_frame_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOscillationEffect, period) == 0x24,
                  room_oscillation_period_offset);

int func_80192F5C(RoomOscillationEffect *effect);

#endif
