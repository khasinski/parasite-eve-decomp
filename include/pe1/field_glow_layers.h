#ifndef PE1_FIELD_GLOW_LAYERS_H
#define PE1_FIELD_GLOW_LAYERS_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"

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

extern FieldGlowSprite D_800F34A8;

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

#endif
