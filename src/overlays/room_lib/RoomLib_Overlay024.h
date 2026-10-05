#ifndef ROOMLIB_OVERLAY024_H
#define ROOMLIB_OVERLAY024_H

#include "common.h"
#include "pe1/field_script_context.h"

typedef struct RoomOverlay024Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} RoomOverlay024Matrix;

typedef struct RoomOverlay024Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} RoomOverlay024Vec4;

typedef struct RoomOverlay024MatrixSeed8 {
    char bytes[8];
} RoomOverlay024MatrixSeed8;

typedef struct RoomOverlay024ViewMatrix {
    s32 m00;
    s32 m04;
    s32 m08;
    s32 m0c;
    s32 m10;
    s32 tx;
    s32 ty;
    s32 tz;
} RoomOverlay024ViewMatrix;

typedef struct RoomOverlay024ViewPosition {
    char transform[0x14];
    s32 base_x;
    s32 base_y;
    s32 base_z;
} RoomOverlay024ViewPosition;

typedef union RoomOverlay024ViewTail {
    RoomOverlay024ViewMatrix matrix;
    RoomOverlay024ViewPosition position;
} RoomOverlay024ViewTail;

typedef struct RoomOverlay024View {
    char pad0[0x1A0];
    RoomOverlay024ViewTail tail;
} RoomOverlay024View;

typedef struct RoomOverlay024VectorSeed {
    s32 words[4];
} RoomOverlay024VectorSeed;

typedef struct RoomOverlay024Effect {
    s16 x;
    s16 y;
    s16 z;
    char pad6[0x2];
    s16 half8;
    s16 velocity_y;
    s16 halfC;
    char padE[0x2];
    s16 timer;
    s16 duration;
    s16 half14;
} RoomOverlay024Effect;

typedef struct RoomOverlay024Transform {
    s32 pad0[5];
    s32 x;
    s32 y;
    s32 z;
} RoomOverlay024Transform;

typedef struct RoomOverlay024Root {
    char pad0[0x238];
    void *view;
} RoomOverlay024Root;

typedef struct RoomOverlay024VariantControl {
    char pad0[2];
    s16 frame;
} RoomOverlay024VariantControl;

typedef struct RoomOverlay024Variant38State {
    s16 final_x;
    s16 final_y;
    s16 final_z;
    char pad6[0xA];
    s16 final_alpha;
    s16 final_scale;
    char pad14[4];
    s16 sparkle_x;
    s16 sparkle_y;
    s16 sparkle_z;
    char pad1E[2];
    s16 sparkle_alpha;
    s16 sparkle_timer;
    s16 resource_selector;
    s16 active_flag;
    s16 drift_value;
    s16 fade_alpha;
    u8 phase;
    char pad2D;
    s16 transform_index;
    s32 field30;
} RoomOverlay024Variant38State;

/* Setup-time view used by the room-specific Variant 38 initializers. */
typedef struct RoomOverlay024Variant38SetupState {
    char pad0[0x22];
    s16 field22;
    s16 resource_selector;
    s16 active_flag;
    s16 height;
    s16 width;
    s16 field2C;
    s16 transform_index;
    s32 field30;
} RoomOverlay024Variant38SetupState;

typedef struct RoomOverlay024Variant290State {
    s16 x;
    s16 y;
    s16 z;
    char pad6[2];
    s16 velocity_x;
    s16 velocity_y;
    s16 velocity_z;
    char padE[2];
    s16 core_alpha;
    s16 core_timer;
    s16 spawn_timer;
    char pad16[2];
    s16 sparkle_x;
    s16 sparkle_y;
    s16 sparkle_z;
    char pad1E[2];
    s16 sparkle_alpha;
    s16 sparkle_timer;
    s16 life_timer;
    s16 active_flag;
    s16 drift_value;
    s16 fade_alpha;
    u8 phase;
    char pad2D;
    s16 transform_index;
    s32 field30;
} RoomOverlay024Variant290State;

typedef struct RoomOverlay024VariantDState {
    s16 x;
    s16 y;
    s16 z;
    char pad6[2];
    s16 velocity_x;
    s16 velocity_y;
    s16 velocity_z;
    char padE[2];
    s16 core_alpha;
    s16 core_timer;
    s16 spawn_timer;
    char pad16[2];
    s16 sparkle_x;
    s16 sparkle_y;
    s16 sparkle_z;
    char pad1E[2];
    s16 sparkle_alpha;
    s16 sparkle_timer;
    s16 life_timer;
    s16 active_flag;
    s16 drift_value;
    s16 fade_alpha;
    u8 phase;
    char pad2D;
    s16 transform_index;
    s32 field30;
    s16 vertical_offset;
} RoomOverlay024VariantDState;

PE1_STATIC_ASSERT(sizeof(RoomOverlay024ViewTail) == 0x20,
                  overlay024_view_tail_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024View, tail) == 0x1A0,
                  overlay024_view_tail_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Root, view) == 0x238,
                  overlay024_root_view_offset);
PE1_STATIC_ASSERT(sizeof(RoomOverlay024Variant38State) == 0x34,
                  overlay024_variant38_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState, field22) == 0x22,
                  overlay024_variant38_setup_field22_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState,
                               resource_selector) == 0x24,
                  overlay024_variant38_setup_resource_selector_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState, active_flag) == 0x26,
                  overlay024_variant38_setup_active_flag_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState, height) == 0x28,
                  overlay024_variant38_setup_height_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState, width) == 0x2A,
                  overlay024_variant38_setup_width_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState, field2C) == 0x2C,
                  overlay024_variant38_setup_field2c_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState,
                               transform_index) == 0x2E,
                  overlay024_variant38_setup_transform_index_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOverlay024Variant38SetupState, field30) == 0x30,
                  overlay024_variant38_setup_field30_offset);
PE1_STATIC_ASSERT(sizeof(RoomOverlay024Variant38SetupState) == 0x34,
                  overlay024_variant38_setup_state_size);
PE1_STATIC_ASSERT(sizeof(RoomOverlay024Variant290State) == 0x34,
                  overlay024_variant290_state_size);
PE1_STATIC_ASSERT(sizeof(RoomOverlay024VariantDState) == 0x38,
                  overlay024_variantd_state_size);

#endif /* ROOMLIB_OVERLAY024_H */
