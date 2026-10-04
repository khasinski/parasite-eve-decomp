#ifndef PE1_RENDER_ANIM_POSITION_H
#define PE1_RENDER_ANIM_POSITION_H

#include "common.h"

/* Position input passed from RenderAnimPlayer.decoderData to the decoder. */
typedef struct RenderAnimPositionInput {
    u8 prefix[2];
    s16 x;
    u8 reserved04[2];
    s16 y;
    u8 reserved08[2];
    s16 z;
} RenderAnimPositionInput;

PE1_STATIC_ASSERT(sizeof(RenderAnimPositionInput) == 0x0C,
                  render_anim_position_input_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimPositionInput, x) == 0x02,
                  render_anim_position_input_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimPositionInput, y) == 0x06,
                  render_anim_position_input_y_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimPositionInput, z) == 0x0A,
                  render_anim_position_input_z_offset);

#endif
