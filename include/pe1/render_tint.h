#ifndef PE1_RENDER_TINT_H
#define PE1_RENDER_TINT_H

#include "common.h"

/* Little-endian PSX DMA packet tag. */
typedef struct RenderGpuTag {
    u32 address : 24;
    u32 length : 8;
} RenderGpuTag;

typedef struct RenderTintTile {
    RenderGpuTag tag;
    u8 r, g, b, command;
    s16 x, y, width, height;
} RenderTintTile;

typedef struct RenderTintMode {
    RenderGpuTag tag;
    u32 command;
} RenderTintMode;

/* Embedded at 0x30 in the state at 0x800BCF88. */
typedef struct RenderTintState {
    RenderTintTile tiles[2];
    RenderTintMode modes[2];
    s16 target_r, target_g, target_b;
    u8 fade_mode, blend_mode;
    s16 start_r, start_g, start_b;
    u16 duration, frame;
} RenderTintState;

PE1_STATIC_ASSERT(sizeof(RenderGpuTag) == 4, render_gpu_tag_size);
PE1_STATIC_ASSERT(sizeof(RenderTintTile) == 16, render_tint_tile_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintTile, r) == 4, render_tint_tile_rgb_offset);
PE1_STATIC_ASSERT(sizeof(RenderTintMode) == 8, render_tint_mode_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintMode, command) == 4, render_tint_command_offset);
PE1_STATIC_ASSERT(sizeof(RenderTintState) == 0x44, render_tint_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintState, modes) == 0x20, render_tint_modes_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintState, target_r) == 0x30, render_tint_target_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintState, fade_mode) == 0x36, render_tint_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintState, start_r) == 0x38, render_tint_start_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintState, duration) == 0x3E, render_tint_duration_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTintState, frame) == 0x40, render_tint_frame_offset);

/* Existing absolute symbols also expose the parameter halfwords/bytes. */
extern s16 D_800BCFE8, D_800BCFEA, D_800BCFEC;
extern s16 D_800BCFF0, D_800BCFF2, D_800BCFF4;
extern u8 D_800BCFEE;
extern u16 D_800BCFF6, D_800BCFF8;
int Render_SetCDDCSlot(void);

#endif
