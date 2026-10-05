#ifndef PE1_FIELD_SHADED_RING_H
#define PE1_FIELD_SHADED_RING_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"
#include "pe1/field_engine_scratch.h"
#include "pe1/field_ring_geometry.h"

/* Field engine ring geometry: the vertex builder writes the outer ring first
 * and the inner ring second; the renderer joins each pair with POLY_G4. */

typedef union FieldRingColor {
    u8 rgb[4];
    u32 word;
} FieldRingColor;

typedef FieldRingGeometry FieldShadedRing;

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
