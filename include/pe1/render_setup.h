#ifndef PE1_RENDER_SETUP_H
#define PE1_RENDER_SETUP_H

/* Declarations used by the model parser Render_SetupEntityPrims. */

#include "pe1/render_object.h"
#include "pe1/gte_types.h"

/* GPU packet head shared by every primitive class: the word count byte of
 * the tag and the command byte after the first colour. */
typedef struct RenderPrimHead {
    u8 address[3];
    u8 length;
    RenderPacketValue colour;
} RenderPrimHead;

/* One double-buffered primitive cursor, stepped by the class being built. */
typedef union RenderPrimCursor {
    u8 *bytes;
    RenderPrimHead *head;
    RenderPacket34 *p34;
    RenderPacket28 *p28;
    RenderPacket24 *p24;
    RenderPacket1C *p1c;
} RenderPrimCursor;

/* Texture records copied into the textured packets: quads use all four
 * UV pairs, triangles stop after the third. */
typedef struct RenderQuadTexture {
    u8 u0, v0;
    u16 clut;
    u8 u1, v1;
    u16 page_bits;
    u8 u2, v2;
    u16 reserved0A;
    u8 u3, v3;
    u16 reserved0E;
} RenderQuadTexture;
typedef struct RenderTriTexture {
    u8 u0, v0;
    u16 clut;
    u8 u1, v1;
    u16 page_bits;
    u8 u2, v2;
    u16 reserved0A;
} RenderTriTexture;
/* Cursor over the consecutive sections of a model block. */
typedef union RenderModelCursor {
    RenderObjectHeader *header;
    RenderObjectPart *parts;
    RenderVec3s *vectors;
    unsigned int *colours;
    RenderPrimitiveDescriptor *descriptors;
    s8 *commands;
    RenderQuadTexture *quad;
    RenderTriTexture *tri;
} RenderModelCursor;

extern int D_8009CDDC;
void Anim_SetInterpRate(RenderObjectEntity *object, int rate);
void Render_InitPrimBlock(RenderObjectEntity *object, s16 x, s16 y, int unused,
                          unsigned int palette_row);

#endif /* PE1_RENDER_SETUP_H */
