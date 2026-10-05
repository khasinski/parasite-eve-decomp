#ifndef PE1_FIELD_SHADED_RING_H
#define PE1_FIELD_SHADED_RING_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"
#include "pe1/field_engine_scratch.h"

/* Field engine ring geometry: the vertex builder writes the outer ring first
 * and the inner ring second; the renderer joins each pair with POLY_G4. */

typedef union FieldRingColor {
    u8 rgb[4];
    u32 word;
} FieldRingColor;

typedef struct FieldRingGeometry {
    /* Shared field-effect geometry prefix and render attributes. */
    /* 0x00 */ GteShortVector *points;
    /* 0x04 */ u8 innerRgb[4];
    /* 0x08 */ u8 outerRgb[4];
    /* 0x0C */ u16 count;
    /* 0x0E */ u16 innerRadius;
    /* 0x10 */ u16 outerRadius;
    /* 0x12 */ s16 depth;
    /* 0x14 */ s16 brightness;
    /* 0x16 */ u16 pad16;
} FieldRingGeometry;

typedef FieldRingGeometry FieldShadedRing;

PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, points) == 0,
                  field_ring_geometry_points_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, innerRgb) == 4,
                  field_ring_geometry_inner_rgb_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, outerRgb) == 8,
                  field_ring_geometry_outer_rgb_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, count) == 0x0C,
                  field_ring_geometry_count_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, innerRadius) == 0x0E,
                  field_ring_geometry_inner_radius_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, outerRadius) == 0x10,
                  field_ring_geometry_outer_radius_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, depth) == 0x12,
                  field_ring_geometry_depth_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, brightness) == 0x14,
                  field_ring_geometry_brightness_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, pad16) == 0x16,
                  field_ring_geometry_pad_offset);
PE1_STATIC_ASSERT(sizeof(FieldRingGeometry) == 0x18,
                  field_ring_geometry_size);

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
