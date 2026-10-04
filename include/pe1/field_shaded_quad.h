#ifndef PE1_FIELD_SHADED_QUAD_H
#define PE1_FIELD_SHADED_QUAD_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"

/* Field engine shaded quad (func_800C499C): four vertex colours scaled by
 * a brightness, a fixed four-corner shape transformed by a placed matrix
 * and drawn as a POLY_G4 (semi-transparent when D_800F337A is set). */

/* Scratchpad work area: the ordering-table depth and the composed matrix. */
typedef struct FieldShadedQuadScratch {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ s32 depth;
    /* 0x10 */ u8 pad10[0xC];
    /* 0x1C */ GteMatrix matrix;
} FieldShadedQuadScratch;

#define FIELD_SHADED_QUAD_SCRATCH ((FieldShadedQuadScratch *)0x1F800000)

extern FieldShadedQuadScratch *D_800F33B4;

typedef struct FieldShadedQuadColors {
    /* 0x00 */ u8 rgb[4][4];
    /* 0x10 */ s16 depth;
    /* 0x12 */ s16 brightness;
} FieldShadedQuadColors;

/* PSY-Q POLY_G4. */
typedef struct FieldShadedQuadPacket {
    RenderGpuTag tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 r1, g1, b1, pad1;
    s16 x1, y1;
    u8 r2, g2, b2, pad2;
    s16 x2, y2;
    u8 r3, g3, b3, pad3;
    s16 x3, y3;
} FieldShadedQuadPacket;

extern char *D_800B0E58[];

/* A packet tag seen as the 32-bit address the DMA chain links through. */
typedef union FieldShadedQuadLink {
    RenderGpuTag *tag;
    u32 word;
} FieldShadedQuadLink;

/* Ordering-table entry `depth` of the active buffer. */
#define SHADED_QUAD_OT(depth) \
    ((RenderGpuTag *)(D_800B0E38.ordering[D_8009CDDC] + (depth) * 4))

extern GteShortVector D_800F3310;
extern GteShortVector D_800F3318;
extern GteShortVector D_800F3320;
extern GteShortVector D_800F3328;
extern u8 D_800F337A;

void func_800C608C(int scale, u8 *src, u8 *dst);
void RotTrans(const GteShortVector *v, GteVector *out, s32 *flag);

#endif
