#include "pe1/render_object.h"
void Render_OffsetObjectTextureCoordinates(RenderObjectEntity *object, int du, int dv, int clutOffset) {
    int savedClut = clutOffset;
    RenderPacket34 *quad = (RenderPacket34 *)object->primitive_buffer;
    RenderPacket28 *tri;
    int i, j;
    int triClut;
    for (i = 0; i < object->header->packet34_count; i++) {
        for (j = 0; j < 2; j++, quad++) {
            quad->v0 += dv;
            quad->u0 += du;
            quad->v1 += dv;
            quad->u1 += du;
            quad->v2 += dv;
            quad->u2 += du;
            /* Retail leaves u3 unchanged. */
            quad->v3 += dv;
            quad->clut += (s8)clutOffset;
        }
    }
    triClut = savedClut;
    /* Keep distinct lifetimes for the two signed-byte CLUT conversions. */
    asm("" : "=r"(clutOffset) : "0"(clutOffset));
    tri = (RenderPacket28 *)quad;
    for (i = 0; i < object->header->packet28_count; i++) {
        for (j = 0; j < 2; j++, tri++) {
            tri->v0 += dv;
            tri->u0 += du;
            tri->v1 += dv;
            tri->u1 += du;
            tri->v2 += dv;
            tri->u2 += du;
            tri->clut += (s8)triClut;
        }
    }
}
