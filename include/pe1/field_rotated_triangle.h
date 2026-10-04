#ifndef PE1_FIELD_ROTATED_TRIANGLE_H
#define PE1_FIELD_ROTATED_TRIANGLE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"
#include "pe1/render_object.h"
#include "pe1/field_tile.h"

/* Field engine rotated triangle (func_800D0E88): a gouraud fan blade with
 * its apex at the anchor and its base `length` away in the local XZ plane,
 * turned by a YXZ rotation and drawn as a PSY-Q POLY_G3. */

typedef struct FieldG3Packet {
    /* 0x00 */ RenderGpuTag tag;
    /* 0x04 */ u8 r0, g0, b0, code;
    /* 0x08 */ s16 x0, y0;
    /* 0x0C */ u8 r1, g1, b1, p1;
    /* 0x10 */ s16 x1, y1;
    /* 0x14 */ u8 r2, g2, b2, p2;
    /* 0x18 */ s16 x2, y2;
} FieldG3Packet;

PE1_STATIC_ASSERT(sizeof(FieldG3Packet) == 0x1C, field_g3_packet_size);

/* Black default for a missing corner colour. */
extern RenderColor D_800C22A0;

void SetPolyG3(FieldG3Packet *packet);
void MulRotMatrix(GteMatrix *matrix);

#endif /* PE1_FIELD_ROTATED_TRIANGLE_H */
