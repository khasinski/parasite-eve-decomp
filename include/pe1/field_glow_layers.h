#ifndef PE1_FIELD_GLOW_LAYERS_H
#define PE1_FIELD_GLOW_LAYERS_H

#include "common.h"
#include "pe1/gte_types.h"

/* Field engine glow layers: a placed axis matrix turned by fixed spins
 * and drawn as scaled sprites through func_800C42A4. Three two-layer
 * variants and one four-layer variant share this record. */
typedef struct FieldGlowLayers {
    /* 0x00 */ u8 pad00[4];
    /* 0x04 */ u16 depth;
    /* 0x06 */ u8 pad06[2];
    /* 0x08 */ s16 x;
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 z;
    /* 0x0E */ u8 pad0E[2];
    /* 0x10 */ GteMatrix matrix;
} FieldGlowLayers;

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

extern u8 D_800F3422;

extern FieldGlowSprite D_800F3498;
extern FieldGlowSprite D_800F34A8;
extern FieldGlowSprite D_800F34B8;
extern FieldGlowSprite D_800F34C8;
/* The four-layer glow writes D_800F34C8's offset and depth as scalars. */
extern s16 D_800F34D0;
extern s16 D_800F34D2;

extern GteShortVector D_800C2154;
extern GteShortVector D_800C215C;
extern GteVector D_800C2164;
extern GteShortVector D_800C2184;
extern GteShortVector D_800C218C;
extern GteVector D_800C2194;
extern GteShortVector D_800C21A4;
extern GteShortVector D_800C21AC;
extern GteVector D_800C21B4;
extern GteShortVector D_800C21D4;
extern GteShortVector D_800C21DC;
extern GteShortVector D_800C21E4;
extern GteShortVector D_800C21EC;
extern GteVector D_800C21F4;

void func_800C2EAC(int arg0);
void func_800C2FF0(int width, int height);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(FieldGlowSprite *sprite, GteMatrix *matrix, u8 mode);

#endif
