#ifndef PE1_FIELD_ROTATED_QUAD_H
#define PE1_FIELD_ROTATED_QUAD_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"

/* Field engine rotated quad (func_800D2370): a gouraud textured rectangle
 * spanned in the local XZ plane, turned by a YXZ rotation about a projected
 * anchor and drawn as a PSY-Q POLY_GT4. */

typedef struct FieldGt4Packet {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u8 r0, g0, b0, code;
    /* 0x08 */ s16 x0, y0;
    /* 0x0C */ u8 u0, v0;
    /* 0x0E */ u16 clut;
    /* 0x10 */ u8 r1, g1, b1, p1;
    /* 0x14 */ s16 x1, y1;
    /* 0x18 */ u8 u1, v1;
    /* 0x1A */ u16 tpage;
    /* 0x1C */ u8 r2, g2, b2, p2;
    /* 0x20 */ s16 x2, y2;
    /* 0x24 */ u8 u2, v2;
    /* 0x26 */ u16 pad2;
    /* 0x28 */ u8 r3, g3, b3, p3;
    /* 0x2C */ s16 x3, y3;
    /* 0x30 */ u8 u3, v3;
    /* 0x32 */ u16 pad3;
} FieldGt4Packet;

PE1_STATIC_ASSERT(sizeof(FieldGt4Packet) == 0x34, field_gt4_packet_size);

/* Black default for a missing corner colour. */
extern RenderColor D_800C22A0;
/* Texture page of the current sprite parameter block. */
extern u16 D_800F3370;

void SetPolyGT4(FieldGt4Packet *packet);
void MulRotMatrix(GteMatrix *matrix);
void AddPrim(unsigned int *orderingEntry, unsigned int *primitive);

#endif /* PE1_FIELD_ROTATED_QUAD_H */
