#ifndef PE1_FIELD_GLOW_LINE_H
#define PE1_FIELD_GLOW_LINE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"
#include "pe1/field_tile.h"

/* Field engine glow line (func_800D2B58): a gouraud line between two
 * projected points plus a textured glow quad laid across it. */

/* PSY-Q LINE_G2. */
typedef struct FieldLineG2Packet {
    /* 0x00 */ RenderGpuTag tag;
    /* 0x04 */ u8 r0, g0, b0, code;
    /* 0x08 */ s16 x0, y0;
    /* 0x0C */ u8 r1, g1, b1, p1;
    /* 0x10 */ s16 x1, y1;
} FieldLineG2Packet;

/* PSY-Q POLY_GT4. */
typedef struct FieldGlowQuadPacket {
    /* 0x00 */ RenderGpuTag tag;
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
} FieldGlowQuadPacket;

PE1_STATIC_ASSERT(sizeof(FieldLineG2Packet) == 0x14, field_line_g2_size);
PE1_STATIC_ASSERT(sizeof(FieldGlowQuadPacket) == 0x34, field_glow_quad_size);

/* Texture page of the shared effect sheet. */
extern u16 D_800E2852;

int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);
u16 GetClut(int x, int y);

#endif /* PE1_FIELD_GLOW_LINE_H */
