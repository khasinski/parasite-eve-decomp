#ifndef PE1_RENDER_LIGHTING_H
#define PE1_RENDER_LIGHTING_H

#include "pe1/gte_types.h"

/* MATRIX arithmetic and the packed word transfers used by the GTE. */
typedef union RenderLightingMatrix {
    GteMatrix matrix;
    u32 words[8];
} RenderLightingMatrix;

extern RenderLightingMatrix D_800BEA40, D_800BEA60;
/* Existing absolute symbols expose individual matrix elements. */
extern s16 D_800BEA42, D_800BEA44, D_800BEA46, D_800BEA48, D_800BEA4A;
extern s16 D_800BEA4C, D_800BEA4E, D_800BEA50;
extern s16 D_800BEA62, D_800BEA64, D_800BEA66, D_800BEA68, D_800BEA6A;
extern s16 D_800BEA6C, D_800BEA6E, D_800BEA70;
extern u8 D_800BD025, D_800BD026, D_800BD027;

int Render_InitRoomPrimState(void *object);

#endif
