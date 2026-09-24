#ifndef ROOM_M273_H
#define ROOM_M273_H

#include "pe1/render_object.h"

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
} RoomM273PulseInput;

PE1_STATIC_ASSERT(sizeof(RoomM273PaletteWord) == 4, room_m273_palette_word_size);
PE1_STATIC_ASSERT(sizeof(RoomM273PaletteEffect) == 16, room_m273_palette_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PaletteInput, size) == 4,
                  room_m273_palette_input_size_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM273PulseInput, velocity) == 8,
                  room_m273_pulse_velocity_offset);

extern RoomM273TrigEntry D_800966EC[];
extern s16 D_8019AE98;
extern int D_8019ABFC[];
extern s16 D_800F336A;
extern s16 D_8019ACC0[];
extern void *D_8019AE94;
extern RoomM273PaletteWord D_8019AC30[];
extern u16 D_800942EC;
extern char D_8019AB70[];
extern char D_8019ACC8[];
extern char D_8019ACCC[];
RoomM273PaletteEffect *func_800CE610(void *pool);
int Inv_ScrambleGrid(void);

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

typedef struct RoomPlacementState {
    u8 pad_00[0x34];
    s32 fallback_x;
    s32 fallback_z;
    u8 pad_3c[3];
    u8 active;
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

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, actor) == 0x08,
                  room_selection_actor_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, matrix) == 0x1c,
                  room_selection_matrix_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSelectionState, activated) == 0x4b,
                  room_selection_activated_offset);

void func_80192664(RoomSelectionState *state);

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementOwner, map) == 0x238,
                  room_placement_map_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementMap, x) == 0x594,
                  room_placement_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomPlacementState, active) == 0x3F,
                  room_placement_active_offset);

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
