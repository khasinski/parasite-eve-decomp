#ifndef PE1_FIELD_TEXTURED_STRIP_H
#define PE1_FIELD_TEXTURED_STRIP_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"
#include "pe1/field_engine_scratch.h"

/* Field engine textured strip (func_800C5A40): consecutive nodes joined by
 * POLY_FT4 segments, each node's two edge points projected with
 * RotTransPers4 and textured from a 16x16 cell grid. */

typedef struct FieldStripNode {
    /* 0x00 */ u8 visible;
    /* 0x01 */ u8 pad01[2];
    /* 0x03 */ u8 cellStep;
    /* 0x04 */ s16 brightness;
    /* 0x06 */ u8 pad06[0xA];
    /* 0x10 */ u8 rgb[4];
    /* 0x14 */ GteShortVector edgeA;
    /* 0x1C */ GteShortVector edgeB;
    /* 0x24 */ u8 pad24[0x20];
} FieldStripNode;

typedef struct FieldTexturedStrip {
    /* 0x00 */ FieldStripNode *nodes;
    /* 0x04 */ s16 count;
    /* 0x06 */ u8 pad06[0xA];
    /* 0x10 */ u8 cell;
    /* 0x11 */ u8 clut;
} FieldTexturedStrip;

/* PSY-Q POLY_FT4. */
typedef struct FieldStripPacket {
    RenderGpuTag tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    s16 x1, y1;
    u8 u1, v1;
    u16 tpage;
    s16 x2, y2;
    u8 u2, v2;
    u16 pad2;
    s16 x3, y3;
    u8 u3, v3;
    u16 pad3;
} FieldStripPacket;

typedef union FieldStripLink {
    RenderGpuTag *tag;
    u32 word;
} FieldStripLink;

#define STRIP_OT(depth) \
    ((RenderGpuTag *)(D_800B0E38.ordering[D_8009CDDC] + (depth) * 4))

extern char *D_800B0E58[];
extern u8 D_800F345C;
extern u8 D_800F345D;
extern u16 D_800E27AC;
extern u16 D_800F341C;
extern u16 D_800F341E;
extern u8 D_800F337A;

void func_800C608C(int scale, u8 *src, u8 *dst);
long RotTransPers4(void *v0, void *v1, void *v2, void *v3, void *sxy0,
                   void *sxy1, void *sxy2, void *sxy3, void *p, void *flag);
u16 GetClut(int x, int y);

#endif
