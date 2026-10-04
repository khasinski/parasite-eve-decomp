#ifndef PE1_FIELD_SHADED_RING_H
#define PE1_FIELD_SHADED_RING_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"
#include "pe1/field_engine_scratch.h"

/* Field engine shaded ring (func_800C4FC4): two rings of `count` points
 * (inner first, outer after it) joined by POLY_G4 segments, each with its
 * own draw-mode packet, coloured by two brightness-scaled colours. */

typedef union FieldRingColor {
    u8 rgb[4];
    u32 word;
} FieldRingColor;

typedef struct FieldShadedRing {
    /* 0x00 */ GteShortVector *points;
    /* 0x04 */ u8 innerRgb[4];
    /* 0x08 */ u8 outerRgb[4];
    /* 0x0C */ u16 count;
    /* 0x0E */ u8 pad0E[4];
    /* 0x12 */ s16 depth;
    /* 0x14 */ s16 brightness;
} FieldShadedRing;

/* PSY-Q POLY_G4 with each colour word addressable as a whole. */
typedef struct FieldRingPacket {
    RenderGpuTag tag;
    FieldRingColor c0;
    s16 x0, y0;
    FieldRingColor c1;
    s16 x1, y1;
    FieldRingColor c2;
    s16 x2, y2;
    FieldRingColor c3;
    s16 x3, y3;
} FieldRingPacket;

typedef union FieldRingLink {
    RenderGpuTag *tag;
    char *bytes;
    u32 word;
} FieldRingLink;

#define RING_OT(depth) \
    ((RenderGpuTag *)(D_800B0E38.ordering[D_8009CDDC] + (depth) * 4))

extern char *D_800B0E58[];
extern u8 D_800E224C;
extern u8 D_800F337A;

void func_800C608C(int scale, u8 *src, u8 *dst);
void RotTrans(const GteShortVector *v, GteVector *out, s32 *flag);
long RotTransPers4(void *v0, void *v1, void *v2, void *v3, void *sxy0,
                   void *sxy1, void *sxy2, void *sxy3, void *p, void *flag);

#endif
