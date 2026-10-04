#ifndef PE1_FIELD_TEXTURED_MODEL_H
#define PE1_FIELD_TEXTURED_MODEL_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"

/* Field engine textured model (func_800C71E4): flat and gouraud textured
 * triangles and quads whose vertex, colour and face records are linked by
 * byte offsets from the model start. Back faces are culled by NCLIP. */

/* A vertex: the position and the byte offset of its colour word. */
typedef struct FieldModelVertex {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ u16 color;
} FieldModelVertex;

typedef union FieldModelColor {
    u32 word;
    struct {
        u8 r, g, b, code;
    } rgb;
} FieldModelColor;

typedef struct FieldModelTri {
    /* 0x00 */ u16 vertex[3];
    /* 0x06 */ u16 uv[3];
} FieldModelTri;

typedef struct FieldModelQuad {
    /* 0x00 */ u16 vertex[4];
    /* 0x08 */ u16 uv[4];
} FieldModelQuad;

typedef struct FieldModelHeader {
    /* 0x00 */ u16 flatTris;
    /* 0x02 */ u16 shadedTris;
    /* 0x04 */ u16 flatQuads;
    /* 0x06 */ u16 shadedQuads;
    /* 0x08 */ u8 pad08[8];
    /* 0x10 */ FieldModelTri tris[1];
} FieldModelHeader;

typedef union FieldTexturedModel {
    FieldModelHeader header;
    u8 bytes[1];
} FieldTexturedModel;

/* PSY-Q POLY_FT3. */
typedef struct FieldModelFt3Packet {
    RenderGpuTag tag;
    FieldModelColor color0;
    s16 x0, y0;
    u16 uv0;
    u16 clut;
    s16 x1, y1;
    u16 uv1;
    u16 tpage;
    s16 x2, y2;
    u16 uv2;
    u16 pad2;
} FieldModelFt3Packet;

/* PSY-Q POLY_GT3. */
typedef struct FieldModelGt3Packet {
    RenderGpuTag tag;
    FieldModelColor color0;
    s16 x0, y0;
    u16 uv0;
    u16 clut;
    FieldModelColor color1;
    s16 x1, y1;
    u16 uv1;
    u16 tpage;
    FieldModelColor color2;
    s16 x2, y2;
    u16 uv2;
    u16 pad2;
} FieldModelGt3Packet;

/* PSY-Q POLY_FT4. */
typedef struct FieldModelFt4Packet {
    RenderGpuTag tag;
    FieldModelColor color0;
    s16 x0, y0;
    u16 uv0;
    u16 clut;
    s16 x1, y1;
    u16 uv1;
    u16 tpage;
    s16 x2, y2;
    u16 uv2;
    u16 pad2;
    s16 x3, y3;
    u16 uv3;
    u16 pad3;
} FieldModelFt4Packet;

/* PSY-Q POLY_GT4. */
typedef struct FieldModelGt4Packet {
    RenderGpuTag tag;
    FieldModelColor color0;
    s16 x0, y0;
    u16 uv0;
    u16 clut;
    FieldModelColor color1;
    s16 x1, y1;
    u16 uv1;
    u16 tpage;
    FieldModelColor color2;
    s16 x2, y2;
    u16 uv2;
    u16 pad2;
    FieldModelColor color3;
    s16 x3, y3;
    u16 uv3;
    u16 pad3;
} FieldModelGt4Packet;

typedef union FieldModelLink {
    RenderGpuTag *tag;
    u32 word;
} FieldModelLink;

/* Scratchpad work area of the model draw. */
typedef struct FieldModelScratch {
    /* 0x00 */ u8 pad00[0x18];
    /* 0x18 */ s32 depth;
    /* 0x1C */ u8 pad1C[4];
    /* 0x20 */ s32 winding;
    /* 0x24 */ u16 tpage;
    /* 0x26 */ u16 clut;
    /* 0x28 */ u8 pad28[0xC];
    /* 0x34 */ u16 semiTrans;
    /* 0x36 */ u8 pad36[2];
    /* 0x38 */ GteMatrix matrix;
} FieldModelScratch;

#define FIELD_MODEL_SCRATCH ((FieldModelScratch *)0x1F800000)

/* Ordering-table entry `depth` of the active buffer. */
#define TEXTURED_MODEL_OT(depth) \
    ((RenderGpuTag *)(D_800B0E38.ordering[D_8009CDDC] + (depth) * 4))

typedef struct FieldModelViewSlot {
    GteMatrix *matrix;
} FieldModelViewSlot;

extern FieldModelViewSlot D_800BCFA4;
extern u16 D_800F346C;
extern u16 D_800F3414;
extern u16 D_800F33E4;
extern s16 D_800F3420;

GteMatrix *CompMatrix(GteMatrix *m0, GteMatrix *m1, GteMatrix *m2);
void SetRotMatrix(GteMatrix *matrix);
void SetTransMatrix(GteMatrix *matrix);

#endif
