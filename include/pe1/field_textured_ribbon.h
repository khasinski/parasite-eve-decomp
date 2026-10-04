#ifndef PE1_FIELD_TEXTURED_RIBBON_H
#define PE1_FIELD_TEXTURED_RIBBON_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"
#include "pe1/render_object.h"

/* Field engine textured ribbon (func_800D3114): a strip of POLY_GT4 quads
 * along a point list, each joint widened across the screen-space direction
 * of its segment and shaded between two colours. */

/* A packet colour seen as bytes or as the whole word copied between
 * corners; the three-byte array keeps the union in memory (BLKmode), so
 * byte stores stay single `sb`s. */
typedef struct FieldRibbonBytes {
    u8 rgb[3];
    u8 code;
} FieldRibbonBytes;

typedef union FieldRibbonColor {
    FieldRibbonBytes bytes;
    u32 word;
} FieldRibbonColor;

typedef struct FieldScreenPoint {
    s16 x, y;
} FieldScreenPoint;

/* A projected corner, copied whole from the previous quad. */
typedef union FieldRibbonPoint {
    FieldScreenPoint xy;
    u32 word;
} FieldRibbonPoint;

/* PSY-Q POLY_GT4. */
typedef struct FieldRibbonPacket {
    /* 0x00 */ RenderGpuTag tag;
    /* 0x04 */ FieldRibbonColor c0;
    /* 0x08 */ FieldRibbonPoint p0;
    /* 0x0C */ u8 u0, v0;
    /* 0x0E */ u16 clut;
    /* 0x10 */ FieldRibbonColor c1;
    /* 0x14 */ FieldRibbonPoint p1;
    /* 0x18 */ u8 u1, v1;
    /* 0x1A */ u16 tpage;
    /* 0x1C */ FieldRibbonColor c2;
    /* 0x20 */ FieldRibbonPoint p2;
    /* 0x24 */ u8 u2, v2;
    /* 0x26 */ u16 pad2;
    /* 0x28 */ FieldRibbonColor c3;
    /* 0x2C */ FieldRibbonPoint p3;
    /* 0x30 */ u8 u3, v3;
    /* 0x32 */ u16 pad3;
} FieldRibbonPacket;

PE1_STATIC_ASSERT(sizeof(FieldRibbonColor) == 4, field_ribbon_color_size);
PE1_STATIC_ASSERT(sizeof(FieldRibbonPacket) == 0x34, field_ribbon_packet_size);

void AddPrim(unsigned int *orderingEntry, unsigned int *primitive);

#endif /* PE1_FIELD_TEXTURED_RIBBON_H */
