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

/* Draws one model of a bank like FxCommon_DrawModel, but darkens the gouraud
 * colours to a quarter and tints every textured primitive with the scene
 * tint colour. */
void FxCommon_DrawModelTinted(void *context, FxCommonPolyModel *model, int index)
{
    FxCommonRenderScratchpad *sp;
    FxCommonModelF3 *f3;
    FxCommonModelF4 *f4;
    FxCommonModelG3 *g3;
    FxCommonModelG4 *g4;
    FxCommonModelFt3 *ft3;
    FxCommonModelFt4 *ft4;
    FxCommonModelGt3 *gt3;
    FxCommonModelGt4 *gt4;
    FxCommonModelF3Packet *f3Packet;
    FxCommonModelF4Packet *f4Packet;
    FxCommonModelG3Packet *g3Packet;
    FxCommonModelG4Packet *g4Packet;
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
    f3 = (FxCommonModelF3 *)&model->bytes[model->header.offsets[0]];
    f4 = (FxCommonModelF4 *)&model->bytes[model->header.offsets[1]];
    g3 = (FxCommonModelG3 *)&model->bytes[model->header.offsets[2]];
    g4 = (FxCommonModelG4 *)&model->bytes[model->header.offsets[3]];
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
        sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
        if (result > 0 && sp->orderingDepth - 1U < 0xFFF) {
            f3Packet->tag.bytes.length = 4;
            f3Packet->xy0 = sp->screenCoordinates[0];
            f3Packet->xy1 = sp->screenCoordinates[1];
            f3Packet->xy2 = sp->screenCoordinates[2];
            *(u32 *)&f3Packet->rgb = f3->color.word;
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
        sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
        if (result > 0 && sp->orderingDepth - 1U < 0xFFF) {
            f4Packet->tag.bytes.length = 5;
            f4Packet->xy0 = sp->screenCoordinates[0];
            f4Packet->xy1 = sp->screenCoordinates[1];
            f4Packet->xy2 = sp->screenCoordinates[2];
            f4Packet->xy3 = sp->screenCoordinates[3];
            *(u32 *)&f4Packet->rgb = f4->color.word;
            FX_COMMON_LINK_PACKET(f4Packet);
            f4Packet++;
        }
    }

    g3Packet = (FxCommonModelG3Packet *)f4Packet;
    for (i = 0; i < model->header.counts[2]; i++, g3++) {
        result = func_80079384(&g3->vertices[0], &g3->vertices[1], &g3->vertices[2],
                               &sp->screenCoordinates[0], &sp->screenCoordinates[1],
                               &sp->screenCoordinates[2], &sp->perspective, &sp->orderingDepth,
                               &sp->transformFlags);
        sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
        if (result > 0 && sp->orderingDepth - 1U < 0xFFF) {
            g3Packet->tag.bytes.length = 6;
            g3Packet->xy0 = sp->screenCoordinates[0];
            g3Packet->xy1 = sp->screenCoordinates[1];
            g3Packet->xy2 = sp->screenCoordinates[2];
            *(u32 *)&g3Packet->rgb0 = g3->colors[0].word;
            g3Packet->rgb0.r = g3->colors[0].rgb.r >> 2;
            g3Packet->rgb0.g = g3->colors[0].rgb.g >> 2;
            g3Packet->rgb0.b = g3->colors[0].rgb.b >> 2;
            g3Packet->rgb1.r = g3->colors[1].rgb.r >> 2;
            g3Packet->rgb1.g = g3->colors[1].rgb.g >> 2;
            g3Packet->rgb1.b = g3->colors[1].rgb.b >> 2;
            g3Packet->rgb2.r = g3->colors[2].rgb.r >> 2;
            g3Packet->rgb2.g = g3->colors[2].rgb.g >> 2;
            g3Packet->rgb2.b = g3->colors[2].rgb.b >> 2;
            FX_COMMON_LINK_PACKET(g3Packet);
            g3Packet++;
        }
    }

    g4Packet = (FxCommonModelG4Packet *)g3Packet;
    for (i = 0; i < model->header.counts[3]; i++, g4++) {
        result = func_80079414(&g4->vertices[0], &g4->vertices[1], &g4->vertices[2],
                               &g4->vertices[3], &sp->screenCoordinates[0],
                               &sp->screenCoordinates[1], &sp->screenCoordinates[2],
                               &sp->screenCoordinates[3], &sp->perspective, &sp->orderingDepth,
                               &sp->transformFlags);
        sp->orderingDepth += D_801EA5E4;
        if (result > 0 && sp->orderingDepth - 1U < 0xFFF) {
            sp->orderingDepth >>= 2;
            g4Packet->tag.bytes.length = 8;
            g4Packet->xy0 = sp->screenCoordinates[0];
            g4Packet->xy1 = sp->screenCoordinates[1];
            g4Packet->xy2 = sp->screenCoordinates[2];
            g4Packet->xy3 = sp->screenCoordinates[3];
            *(u32 *)&g4Packet->rgb0 = g4->colors[0].word;
            g4Packet->rgb0.r = g4->colors[0].rgb.r >> 2;
            g4Packet->rgb0.g = g4->colors[0].rgb.g >> 2;
            g4Packet->rgb0.b = g4->colors[0].rgb.b >> 2;
            g4Packet->rgb1.r = g4->colors[1].rgb.r >> 2;
            g4Packet->rgb1.g = g4->colors[1].rgb.g >> 2;
            g4Packet->rgb1.b = g4->colors[1].rgb.b >> 2;
            g4Packet->rgb2.r = g4->colors[2].rgb.r >> 2;
            g4Packet->rgb2.g = g4->colors[2].rgb.g >> 2;
            g4Packet->rgb2.b = g4->colors[2].rgb.b >> 2;
            g4Packet->rgb3.r = g4->colors[3].rgb.r >> 2;
            g4Packet->rgb3.g = g4->colors[3].rgb.g >> 2;
            g4Packet->rgb3.b = g4->colors[3].rgb.b >> 2;
            FX_COMMON_LINK_PACKET(g4Packet);
            g4Packet++;
        }
    }

    ft3Packet = (FxCommonModelFt3Packet *)g4Packet;
    for (i = 0; i < model->header.counts[4]; i++, ft3++) {
        gte_ldv3(&ft3->vertices[0], &ft3->vertices[1], &ft3->vertices[2]);
        gte_rtpt_padded();
        gte_nclip();
        gte_stmac0(&sp->normalClip);
        if (sp->normalClip > 0) {
            gte_avsz3_padded();
            gte_stotz(&sp->orderingDepth);
            sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
            if (sp->orderingDepth > 0) {
                gte_stsxy3(&ft3Packet->xy0, &ft3Packet->xy1, &ft3Packet->xy2);
                ft3Packet->tag.bytes.length = 7;
                *(u32 *)&ft3Packet->rgb = ft3->color.word;
                ft3Packet->rgb.r = D_801EA264[0];
                ft3Packet->rgb.g = D_801EA264[1];
                ft3Packet->rgb.b = D_801EA264[2];
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
            sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
            if (sp->orderingDepth > 0) {
                gte_stsxy3(&ft4Packet->xy0, &ft4Packet->xy1, &ft4Packet->xy2);
                gte_ldv0(&ft4->vertices[3]);
                gte_rtps();
                gte_stsxy2(&ft4Packet->xy3);
                ft4Packet->tag.bytes.length = 9;
                *(u32 *)&ft4Packet->rgb = ft4->color.word;
                ft4Packet->rgb.r = D_801EA264[0];
                ft4Packet->rgb.g = D_801EA264[1];
                ft4Packet->rgb.b = D_801EA264[2];
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
            sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
            if (sp->orderingDepth > 0) {
                gte_stsxy3(&gt3Packet->xy0, &gt3Packet->xy1, &gt3Packet->xy2);
                gt3Packet->tag.bytes.length = 9;
                *(u32 *)&gt3Packet->rgb0 = gt3->colors[0].word;
                gt3Packet->rgb0.r = D_801EA264[0];
                gt3Packet->rgb0.g = D_801EA264[1];
                gt3Packet->rgb0.b = D_801EA264[2];
                *(u32 *)&gt3Packet->rgb2 = *(u32 *)&gt3Packet->rgb1 = *(u32 *)D_801EA264;
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
            sp->orderingDepth = (sp->orderingDepth + D_801EA5E4) >> 2;
            if (sp->orderingDepth > 0) {
                gte_stsxy3(&gt4Packet->xy0, &gt4Packet->xy1, &gt4Packet->xy2);
                gte_ldv0(&gt4->vertices[3]);
                gte_rtps();
                gte_stsxy2(&gt4Packet->xy3);
                gt4Packet->tag.bytes.length = 12;
                *(u32 *)&gt4Packet->rgb0 = gt4->colors[0].word;
                gt4Packet->rgb0.r = D_801EA264[0];
                gt4Packet->rgb0.g = D_801EA264[1];
                gt4Packet->rgb0.b = D_801EA264[2];
                *(u32 *)&gt4Packet->rgb3 = *(u32 *)&gt4Packet->rgb2 = *(u32 *)&gt4Packet->rgb1 =
                    *(u32 *)D_801EA264;
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
