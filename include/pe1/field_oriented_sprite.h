#ifndef PE1_FIELD_ORIENTED_SPRITE_H
#define PE1_FIELD_ORIENTED_SPRITE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"
#include "pe1/field_engine_scratch.h"
#include "pe1/field_textured_strip.h"

/* Field engine oriented sprite (func_800C3324): a textured quad built from
 * the fixed four-corner shape, turned by a YXZ rotation, scaled per axis
 * and placed in the view, drawn as a POLY_FT4. */

typedef struct FieldOrientedSprite {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector rotation;
    /* 0x10 */ GteVector scale;
    /* 0x20 */ u8 rgb[4];
    /* 0x24 */ u8 cell;
    /* 0x25 */ u8 clut;
    /* 0x26 */ s16 depth;
    /* 0x28 */ u16 brightness;
} FieldOrientedSprite;

/* Points at the current view matrix. */
typedef struct FieldViewMatrixSlot {
    GteMatrix *matrix;
} FieldViewMatrixSlot;

extern FieldViewMatrixSlot D_800BCFA4;

extern GteShortVector D_800F3310;
extern GteShortVector D_800F3318;
extern GteShortVector D_800F3320;
extern GteShortVector D_800F3328;

#endif
