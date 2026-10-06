#ifndef PE1_FIELD_GLOW_SPRITE_H
#define PE1_FIELD_GLOW_SPRITE_H

#include "common.h"
#include "pe1/gte_types.h"

/* Sprite parameter block (0x10 bytes) drawn by func_800C42A4: colour,
 * texture cell (16x16 grid), CLUT slot, flip flags (1 = U, 2 = V), depth
 * offset and the brightness the colour is scaled by. */
typedef struct FieldGlowSprite {
    /* 0x00 */ u8 r;
    /* 0x01 */ u8 g;
    /* 0x02 */ u8 b;
    /* 0x03 */ u8 pad03;
    /* 0x04 */ u8 cell;
    /* 0x05 */ u8 clut;
    /* 0x06 */ u8 flip;
    /* 0x07 */ u8 pad07;
    /* 0x08 */ s16 offset;
    /* 0x0A */ u16 depth;
    /* 0x0C */ u8 pad0C[4];
} FieldGlowSprite;

/* Draws one scaled glow sprite at the matrix translation (engine/
 * FieldEng_SpriteRendering.c). */
void func_800C42A4(FieldGlowSprite *sprite, GteMatrix *matrix, u8 mode);

#endif
