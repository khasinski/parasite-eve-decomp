#ifndef ROOM_M273_H
#define ROOM_M273_H

#include "pe1/render_object.h"

typedef GteShortVector RoomM273Vector;

typedef struct {
    signed int low : 16;
    signed int high : 16;
} RoomM273TrigEntry;

extern RoomM273TrigEntry D_800966EC[];
extern u16 D_800942EC;
extern char D_8019AB70[];
extern char D_8019ACC8[];
extern char D_8019ACCC[];

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
