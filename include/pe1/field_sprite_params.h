#ifndef PE1_FIELD_SPRITE_PARAMS_H
#define PE1_FIELD_SPRITE_PARAMS_H

#include "common.h"

/* Halfword sprite parameter block at 0x800F3368. Effect controllers fill it
 * before their particles draw: the frame step pair at 0x00/0x02 (read signed
 * by the sprite draws), the CLUT and texture page, the depth and the sprite
 * extents. This is the one declaration of the block; the halfwords at
 * 0x800F336A..0x800F3378 that older units declared one by one are its fields. */
typedef struct RenderEffectParameters {
    u16 parameter00;
    u16 parameter02; /* frame step, read as (s16) */
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RenderEffectParameters;

PE1_STATIC_ASSERT(sizeof(RenderEffectParameters) == 0x12,
                  render_effect_parameters_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderEffectParameters, palette) == 4,
                  render_effect_parameters_palette);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderEffectParameters, depth) == 0xC,
                  render_effect_parameters_depth);
extern RenderEffectParameters g_FieldFxSpriteParams;

/* Standalone names for two of the halfwords. Retail reloads the palette
 * with a fresh lui/lhu after calls, which the field form does not give: a
 * field reached twice across a call keeps its address in a callee-saved
 * register. Units with that reload read D_800F336C. The depth name is still
 * used by fieldfx/geometry.c. */
extern u16 D_800F336C;
extern u16 D_800F2E2C; /* g_FieldFxSpriteParams.palette */
extern s16 D_800F3374;
extern s16 D_800F2E34; /* g_FieldFxSpriteParams.depth */

#endif /* PE1_FIELD_SPRITE_PARAMS_H */
