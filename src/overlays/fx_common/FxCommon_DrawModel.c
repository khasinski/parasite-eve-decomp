#include "fx_common_model.h"
#include "pe1/gte.h"

/* addPrim: chain the packet into the ordering table slot at the depth the
 * projection left in the scratchpad. */
#define FX_COMMON_LINK_PACKET(packet)                                                    \
    {                                                                                    \
        FxCommonBuffer *buffer;                                                          \
        FxCommonAddress address;                                                         \
        buffer = D_8019C9C0;                                                             \
        (packet)->tag.bits.address = buffer->allocation[sp->orderingDepth].bits.address; \
        address.pointer = (packet);                                                      \
        buffer->allocation[sp->orderingDepth].bits.address = address.word;               \
    }

/* Draws every primitive list of one model of a bank with the current GTE
 * matrices: flat triangles and quads through RotTransPers3/4, textured and
 * gouraud-textured ones through inline RTPT with back-face culling. */
void FxCommon_DrawModel(void *context, FxCommonPolyModel *model, int index)
{
    FxCommonRenderScratchpad *sp;
    FxCommonFlatTriangle *f3;
    FxCommonFlatQuad *f4;
    FxCommonModelFt3Short *ft3s;
    FxCommonModelFt4 *ft4s;
    FxCommonModelFt3 *ft3;
    FxCommonModelFt4 *ft4;
    FxCommonModelGt3 *gt3;
    FxCommonModelGt4 *gt4;
    FxCommonModelF3Packet *f3Packet;
    FxCommonModelF4Packet *f4Packet;
    FxCommonModelFt3ShortPacket *ft3sPacket;
    FxCommonModelFt4ShortPacket *ft4sPacket;
    FxCommonModelFt3Packet *ft3Packet;
    FxCommonModelFt4Packet *ft4Packet;
    FxCommonModelGt3Packet *gt3Packet;
    FxCommonModelGt4Packet *gt4Packet;
    int i;
    int result;
    s32 *entry;

    entry = model->bankOffsets + index;
    model = (FxCommonPolyModel *)&model->bytes[*entry];
    f3Packet = (FxCommonModelF3Packet *)D_8019C9C0->data;
    f3 = (FxCommonFlatTriangle *)&model->bytes[model->header.offsets[0]];
    f4 = (FxCommonFlatQuad *)&model->bytes[model->header.offsets[1]];
    ft3s = (FxCommonModelFt3Short *)&model->bytes[model->header.offsets[2]];
    ft4s = (FxCommonModelFt4 *)&model->bytes[model->header.offsets[3]];
    ft3 = (FxCommonModelFt3 *)&model->bytes[model->header.offsets[4]];
    ft4 = (FxCommonModelFt4 *)&model->bytes[model->header.offsets[5]];
    gt3 = (FxCommonModelGt3 *)&model->bytes[model->header.offsets[6]];
    gt4 = (FxCommonModelGt4 *)&model->bytes[model->header.offsets[7]];
    sp = FX_COMMON_SCRATCHPAD;

    for (i = 0; i < model->header.counts[0]; i++, f3++) {
        result = func_80079384(&f3->vertices[0], &f3->vertices[1], &f3->vertices[2],
                               &sp->screenCoordinates[0], &sp->screenCoordinates[1],
                               &sp->screenCoordinates[2], &sp->perspective, &sp->orderingDepth,
                               &sp->transformFlags);
        sp->orderingDepth += D_801EA5E4;
        if (result > 0 && sp->orderingDepth - 1U < 0xFFF) {
            sp->orderingDepth >>= 2;
            f3Packet->tag.bytes.length = 4;
            f3Packet->xy0 = sp->screenCoordinates[0];
            f3Packet->xy1 = sp->screenCoordinates[1];
            f3Packet->xy2 = sp->screenCoordinates[2];
            *(u32 *)&f3Packet->rgb = f3->color;
            FX_COMMON_LINK_PACKET(f3Packet);
            f3Packet++;
        }
    }

    f4Packet = (FxCommonModelF4Packet *)f3Packet;
    for (i = 0; i < model->header.counts[1]; i++, f4++) {
        result = func_80079414(&f4->vertices[0], &f4->vertices[1], &f4->vertices[2],
                               &f4->vertices[3], &sp->screenCoordinates[0],
                               &sp->screenCoordinates[1], &sp->screenCoordinates[2],
                               &sp->screenCoordinates[3], &sp->perspective, &sp->orderingDepth,
                               &sp->transformFlags);
        sp->orderingDepth += D_801EA5E4;
        if (result > 0 && sp->orderingDepth - 1U < 0xFFF) {
            sp->orderingDepth >>= 2;
            f4Packet->tag.bytes.length = 5;
            f4Packet->xy0 = sp->screenCoordinates[0];
            f4Packet->xy1 = sp->screenCoordinates[1];
            f4Packet->xy2 = sp->screenCoordinates[2];
            f4Packet->xy3 = sp->screenCoordinates[3];
            *(u32 *)&f4Packet->rgb = f4->color;
            FX_COMMON_LINK_PACKET(f4Packet);
            f4Packet++;
        }
    }

    ft3sPacket = (FxCommonModelFt3ShortPacket *)f4Packet;
    for (i = 0; i < model->header.counts[2]; i++, ft3s++) {
        result = func_80079384(&ft3s->vertices[0], &ft3s->vertices[1], &ft3s->vertices[2],
                               &sp->screenCoordinates[0], &sp->screenCoordinates[1],
                               &sp->screenCoordinates[2], &sp->perspective, &sp->orderingDepth,
                               &sp->transformFlags);
        sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
        if (result > 0 && sp->orderingDepth - 1U < 0xFFF) {
            ft3sPacket->tag.bytes.length = 6;
            ft3sPacket->xy0 = sp->screenCoordinates[0];
            ft3sPacket->xy1 = sp->screenCoordinates[1];
            ft3sPacket->xy2 = sp->screenCoordinates[2];
            *(u32 *)&ft3sPacket->rgb = ft3s->color;
            *(u32 *)&ft3sPacket->uv0 = ft3s->uv0;
            *(u32 *)&ft3sPacket->uv1 = ft3s->uv1;
            FX_COMMON_LINK_PACKET(ft3sPacket);
            ft3sPacket++;
        }
    }

    ft4sPacket = (FxCommonModelFt4ShortPacket *)ft3sPacket;
    for (i = 0; i < model->header.counts[3]; i++, ft4s++) {
        gte_ldv3(&ft4s->vertices[0], &ft4s->vertices[1], &ft4s->vertices[2]);
        gte_rtpt_padded();
        gte_nclip();
        gte_stmac0(&sp->normalClip);
        if (sp->normalClip > 0) {
            gte_avsz3_padded();
            gte_stotz(&sp->orderingDepth);
            sp->orderingDepth += D_801EA5E4;
            if (sp->orderingDepth > 0) {
                sp->orderingDepth >>= 2;
                gte_stsxy3(&ft4sPacket->xy0, &ft4sPacket->xy1, &ft4sPacket->xy2);
                gte_ldv0(&ft4s->vertices[3]);
                gte_rtps();
                gte_stsxy2(&ft4sPacket->xy3);
                ft4sPacket->tag.bytes.length = 8;
                *(u32 *)&ft4sPacket->rgb = ft4s->color;
                *(u32 *)&ft4sPacket->uv0 = ft4s->uv0;
                *(u32 *)&ft4sPacket->uv1 = ft4s->uv1;
                *(u32 *)&ft4sPacket->uv2 = ft4s->uv23.word;
                FX_COMMON_LINK_PACKET(ft4sPacket);
                ft4sPacket++;
            }
        }
    }
    D_8019C9C0->data = (u8 *)ft4sPacket;

    ft3Packet = (FxCommonModelFt3Packet *)ft4sPacket;
    for (i = 0; i < model->header.counts[4]; i++, ft3++) {
        gte_ldv3(&ft3->vertices[0], &ft3->vertices[1], &ft3->vertices[2]);
        gte_rtpt_padded();
        gte_nclip();
        gte_stmac0(&sp->normalClip);
        if (sp->normalClip > 0) {
            gte_avsz3_padded();
            gte_stotz(&sp->orderingDepth);
            sp->orderingDepth += D_801EA5E4;
            if (sp->orderingDepth > 0) {
                sp->orderingDepth >>= 2;
                gte_stsxy3(&ft3Packet->xy0, &ft3Packet->xy1, &ft3Packet->xy2);
                ft3Packet->tag.bytes.length = 7;
                *(u32 *)&ft3Packet->rgb = ft3->color;
                *(u32 *)&ft3Packet->uv0 = ft3->uv0;
                *(u32 *)&ft3Packet->uv1 = ft3->uv1;
                *(u32 *)&ft3Packet->uv2 = ft3->uv2;
                FX_COMMON_LINK_PACKET(ft3Packet);
                ft3Packet++;
            }
        }
    }

    ft4Packet = (FxCommonModelFt4Packet *)ft3Packet;
    for (i = 0; i < model->header.counts[5]; i++, ft4++) {
        gte_ldv3(&ft4->vertices[0], &ft4->vertices[1], &ft4->vertices[2]);
        gte_rtpt_padded();
        gte_nclip();
        gte_stmac0(&sp->normalClip);
        if (sp->normalClip > 0) {
            gte_avsz3_padded();
            gte_stotz(&sp->orderingDepth);
            sp->orderingDepth += D_801EA5E4;
            if (sp->orderingDepth > 0) {
                sp->orderingDepth >>= 2;
                gte_stsxy3(&ft4Packet->xy0, &ft4Packet->xy1, &ft4Packet->xy2);
                gte_ldv0(&ft4->vertices[3]);
                gte_rtps();
                gte_stsxy2(&ft4Packet->xy3);
                ft4Packet->tag.bytes.length = 9;
                *(u32 *)&ft4Packet->rgb = ft4->color;
                *(u32 *)&ft4Packet->uv0 = ft4->uv0;
                *(u32 *)&ft4Packet->uv1 = ft4->uv1;
                *(u32 *)&ft4Packet->uv2 = ft4->uv23.word;
                *(u16 *)&ft4Packet->u3 = ft4->uv23.halves.uv3;
                FX_COMMON_LINK_PACKET(ft4Packet);
                ft4Packet++;
            }
        }
    }

    gt3Packet = (FxCommonModelGt3Packet *)ft4Packet;
    for (i = 0; i < model->header.counts[6]; i++, gt3++) {
        gte_ldv3(&gt3->vertices[0], &gt3->vertices[1], &gt3->vertices[2]);
        gte_rtpt_padded();
        gte_nclip();
        gte_stmac0(&sp->normalClip);
        if (sp->normalClip > 0) {
            gte_avsz3_padded();
            gte_stotz(&sp->orderingDepth);
            sp->orderingDepth += D_801EA5E4;
            if (sp->orderingDepth > 0) {
                sp->orderingDepth >>= 2;
                gte_stsxy3(&gt3Packet->xy0, &gt3Packet->xy1, &gt3Packet->xy2);
                gt3Packet->tag.bytes.length = 9;
                *(u32 *)&gt3Packet->rgb0 = gt3->colors[0];
                *(u32 *)&gt3Packet->rgb1 = gt3->colors[1];
                *(u32 *)&gt3Packet->rgb2 = gt3->colors[2];
                *(u32 *)&gt3Packet->uv0 = gt3->uv0;
                *(u32 *)&gt3Packet->uv1 = gt3->uv1;
                *(u32 *)&gt3Packet->uv2 = gt3->uv2;
                FX_COMMON_LINK_PACKET(gt3Packet);
                gt3Packet++;
            }
        }
    }

    gt4Packet = (FxCommonModelGt4Packet *)gt3Packet;
    for (i = 0; i < model->header.counts[7]; i++, gt4++) {
        gte_ldv3(&gt4->vertices[0], &gt4->vertices[1], &gt4->vertices[2]);
        gte_rtpt_padded();
        gte_nclip();
        gte_stmac0(&sp->normalClip);
        if (sp->normalClip > 0) {
            gte_avsz3_padded();
            gte_stotz(&sp->orderingDepth);
            sp->orderingDepth += D_801EA5E4;
            if (sp->orderingDepth > 0) {
                sp->orderingDepth >>= 2;
                gte_stsxy3(&gt4Packet->xy0, &gt4Packet->xy1, &gt4Packet->xy2);
                gte_ldv0(&gt4->vertices[3]);
                gte_rtps();
                gte_stsxy2(&gt4Packet->xy3);
                gt4Packet->tag.bytes.length = 12;
                *(u32 *)&gt4Packet->rgb0 = gt4->colors[0];
                *(u32 *)&gt4Packet->rgb1 = gt4->colors[1];
                *(u32 *)&gt4Packet->rgb2 = gt4->colors[2];
                *(u32 *)&gt4Packet->rgb3 = gt4->colors[3];
                *(u32 *)&gt4Packet->uv0 = gt4->uv0;
                *(u32 *)&gt4Packet->uv1 = gt4->uv1;
                *(u32 *)&gt4Packet->uv2 = gt4->uv23.word;
                *(u16 *)&gt4Packet->u3 = gt4->uv23.halves.uv3;
                FX_COMMON_LINK_PACKET(gt4Packet);
                gt4Packet++;
            }
        }
    }
    D_8019C9C0->data = (u8 *)gt4Packet;
}
