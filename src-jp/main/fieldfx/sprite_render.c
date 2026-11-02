#include "common.h"
#include "pe1/field_billboard.h"
#include "pe1/field_glow_layers.h"
#include "pe1/field_oriented_sprite.h"
#include "pe1/field_particle_chain.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_shaded_ring.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_textured_strip.h"
#include "pe1/geom_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_bios.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"

/* Field engine sprite rendering (0x800C2EAC..0x800C5EB0): the draw state
 * (texture page, CLUT origin, blend mode, cell quad, colour ramps) and the
 * sprite draws that use it: oriented sprite, billboard, glow sprite, shaded
 * quad, shaded ring, textured strip and the particle chain. They share the
 * draw state and scratch pointers private to this range (main_tu_evidence
 * G1390). */

void FieldFx_SetSpriteTexturePreset(u8 mode) {
    if (mode == 0) {
        g_FieldFxSpriteTpageX = 0x340;
        g_FieldFxSpriteTpageY = 0x100;
        g_FieldFxSpriteClutX = 0;
        g_FieldFxSpriteClutY = 0x1D7;
        g_FieldFxSpriteCellVBase = 0;
    }
    if (mode == 1) {
        g_FieldFxSpriteTpageX = 0x340;
        g_FieldFxSpriteTpageY = 0x160;
        g_FieldFxSpriteClutY = 0x1DB;
        g_FieldFxSpriteClutX = 0;
        g_FieldFxSpriteCellVBase = 0x60;
    }
    if (mode == 2) {
        g_FieldFxSpriteTpageX = 0x340;
        g_FieldFxSpriteTpageY = 0x100;
        g_FieldFxSpriteClutX = 0;
        g_FieldFxSpriteClutY = 0x1D6;
        g_FieldFxSpriteCellVBase = 0;
    }
    if (mode == 3) {
        g_FieldFxSpriteTpageX = 0x380;
        g_FieldFxSpriteTpageY = 0x100;
        g_FieldFxSpriteClutX = 0;
        g_FieldFxSpriteClutY = 0x1C8;
        g_FieldFxSpriteCellVBase = 0;
    }

    g_FieldFxSpriteTpageValue = GetTPage(g_FieldFxSpriteColorMode, g_FieldFxSpriteBlendRate, g_FieldFxSpriteTpageX, g_FieldFxSpriteTpageY);
}

void FieldFx_SetSpriteCellSize(int width, int height) {
    u8 widthMinus;
    u8 heightMinus;
    int x;
    int y;

    widthMinus = width - 1;
    heightMinus = height - 1;

    g_FieldFxSpriteCellUMax = width;
    x = (width & 0xFF) << 4;
    g_FieldFxSpriteCellVExtent = height;
    y = (height & 0xFF) << 4;

    g_FieldFxCellQuad[0].x = -x;
    g_FieldFxCellQuad[0].y = -y;
    g_FieldFxCellQuad[0].z = 0;
    g_FieldFxCellQuad[1].x = x;
    g_FieldFxCellQuad[1].y = -y;
    g_FieldFxCellQuad[1].z = 0;
    g_FieldFxCellQuad[2].x = -x;
    g_FieldFxCellQuad[2].y = y;
    g_FieldFxCellQuad[2].z = 0;
    g_FieldFxCellQuad[3].x = x;
    g_FieldFxCellQuad[3].y = y;
    g_FieldFxCellQuad[3].z = 0;

    g_FieldFxSpriteCellUMax = widthMinus;
    g_FieldFxSpriteCellVExtent = heightMinus;
}

void FieldFx_SetSpriteColorMode(int mode) {
    mode = (short)mode;

    switch (mode) {
    case 0x10:
        g_FieldFxSpriteColorMode = 0;
        break;
    case 0x100:
        g_FieldFxSpriteColorMode = 1;
        break;
    default:
        printf(g_FieldSpriteWrongColorModeMsg);
        break;
    }

    g_FieldFxSpriteTpageValue = GetTPage(g_FieldFxSpriteColorMode, g_FieldFxSpriteBlendRate, g_FieldFxSpriteTpageX, g_FieldFxSpriteTpageY);
}

void FieldFx_InterpolateColorRamp(u8 *table, u32 step, u8 *out) {
    u32 sum = 0;
    u32 one = 0x1000;
    u32 last = 0xFF;
    u8 *entry = table;

    while (1) {
        u32 duration = entry[3];
        u8 *next = entry + 4;

        sum += duration;
        if (step < sum) {
            u32 weight = ((sum - step) << 12) / duration;
            u32 inv = one - weight;
            u32 out0;
            u32 out1;
            u32 out2;

            out0 = ((inv * entry[4]) + (weight * entry[0])) >> 12;
            out1 = ((inv * entry[5]) + (weight * entry[1])) >> 12;
            out2 = ((inv * entry[6]) + (weight * entry[2])) >> 12;
            out[0] = out0;
            out[1] = out1;
            out[2] = out2;
            return;
        }

        if (entry[7] == last) {
            out[0] = entry[4];
            out[1] = entry[5];
            out[2] = entry[6];
            return;
        }

        entry = next;
    }
}

void FieldFx_SetSpriteBlendMode(u8 mode) {
    g_FieldFxSpriteBlendMode = mode;
    switch (mode) {
    case 0:
        g_FieldFxSpriteSemiTrans = 0;
        g_FieldFxSpriteBlendRate = 0;
        break;
    case 1:
        g_FieldFxSpriteSemiTrans = 1;
        g_FieldFxSpriteBlendRate = 0;
        break;
    case 2:
        g_FieldFxSpriteSemiTrans = 1;
        g_FieldFxSpriteBlendRate = 1;
        break;
    case 3:
        g_FieldFxSpriteSemiTrans = 1;
        g_FieldFxSpriteBlendRate = 2;
        break;
    case 4:
        g_FieldFxSpriteSemiTrans = 1;
        g_FieldFxSpriteBlendRate = 3;
        break;
    }
    g_FieldFxSpriteTpageValue = GetTPage(g_FieldFxSpriteColorMode, g_FieldFxSpriteBlendRate, g_FieldFxSpriteTpageX, g_FieldFxSpriteTpageY);
}

void FieldFx_DrawOrientedSprite(FieldOrientedSprite *sprite)
{
    FieldStripPacket *packet;
    FieldStripLink link;

    packet = (FieldStripPacket *)(g_RenderAnimBufferBase[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_FieldFxScratch = FIELDFX_SCRATCH;
    g_FieldFxScratch->v = sprite->cell & 0xF0;
    g_FieldFxScratch->u = (sprite->cell - g_FieldFxScratch->v) << 4;
    g_FieldFxScratch->clutX = sprite->clut << 4;
    g_FieldFxScratch->clutY = sprite->clut >> 4;
    if (sprite->brightness != 0x80) {
        g_FieldFxScratch->channel = sprite->rgb[0];
        g_FieldFxScratch->channel *= sprite->brightness;
        if (g_FieldFxScratch->channel > 0x7FFF) {
            g_FieldFxScratch->channel = 0x7FFF;
        }
        g_FieldFxScratch->channel >>= 7;
        packet->r0 = g_FieldFxScratch->channel;
        g_FieldFxScratch->channel = sprite->rgb[1];
        g_FieldFxScratch->channel *= sprite->brightness;
        if (g_FieldFxScratch->channel > 0x7FFF) {
            g_FieldFxScratch->channel = 0x7FFF;
        }
        g_FieldFxScratch->channel >>= 7;
        packet->g0 = g_FieldFxScratch->channel;
        g_FieldFxScratch->channel = sprite->rgb[2];
        g_FieldFxScratch->channel *= sprite->brightness;
        if (g_FieldFxScratch->channel > 0x7FFF) {
            g_FieldFxScratch->channel = 0x7FFF;
        }
        g_FieldFxScratch->channel >>= 7;
        packet->b0 = g_FieldFxScratch->channel;
    } else {
        packet->r0 = sprite->rgb[0];
        packet->g0 = sprite->rgb[1];
        packet->b0 = sprite->rgb[2];
    }
    RotMatrixYXZ(&sprite->rotation, &g_FieldFxScratch->local);
    ScaleMatrix(&g_FieldFxScratch->local, &sprite->scale);
    g_FieldFxScratch->local.t[0] = 0;
    g_FieldFxScratch->local.t[1] = 0;
    g_FieldFxScratch->local.t[2] = 0;
    g_FieldFxScratch->matrix = *g_ViewMatrixSlot.value;
    g_FieldFxScratch->matrix.t[0] += (g_FieldFxScratch->matrix.m[0][2] * sprite->position.z +
                                g_FieldFxScratch->matrix.m[0][1] * sprite->position.y +
                                g_FieldFxScratch->matrix.m[0][0] * sprite->position.x) / 4096;
    g_FieldFxScratch->matrix.t[1] += (g_FieldFxScratch->matrix.m[1][2] * sprite->position.z +
                                g_FieldFxScratch->matrix.m[1][1] * sprite->position.y +
                                g_FieldFxScratch->matrix.m[1][0] * sprite->position.x) / 4096;
    g_FieldFxScratch->matrix.t[2] += (g_FieldFxScratch->matrix.m[2][2] * sprite->position.z +
                                g_FieldFxScratch->matrix.m[2][1] * sprite->position.y +
                                g_FieldFxScratch->matrix.m[2][0] * sprite->position.x) / 4096;
    gte_CompMatrix(&g_FieldFxScratch->matrix, &g_FieldFxScratch->local, &g_FieldFxScratch->matrix);
    gte_ldrotmatrix(&g_FieldFxScratch->matrix);
    gte_ldtransmatrix(&g_FieldFxScratch->matrix);
    gte_ldv3(g_FieldFxCellQuad, &g_FieldFxCellQuad[1], &g_FieldFxCellQuad[2]);
    gte_rtpt_padded();
    packet->tag.length = 9;
    packet->code = 0x2C;
    packet->u0 = g_FieldFxScratch->u;
    packet->v0 = g_FieldFxScratch->v;
    packet->u1 = g_FieldFxScratch->u + g_FieldFxSpriteCellUMax;
    packet->v1 = g_FieldFxScratch->v;
    packet->u2 = g_FieldFxScratch->u;
    packet->v2 = g_FieldFxScratch->v + g_FieldFxSpriteCellVExtent;
    packet->u3 = g_FieldFxScratch->u + g_FieldFxSpriteCellUMax;
    packet->v3 = g_FieldFxScratch->v + g_FieldFxSpriteCellVExtent;
    gte_avsz3_padded();
    /* PSY-Q setSemiTrans(packet, g_FieldFxSpriteSemiTrans). */
    if (g_FieldFxSpriteSemiTrans) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&g_FieldFxScratch->depth);
    gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
    gte_ldv0(&g_FieldFxCellQuad[3]);
    gte_rtps();
    packet->tpage = g_FieldFxSpriteTpageValue;
    packet->clut = GetClut(g_FieldFxSpriteClutX + g_FieldFxScratch->clutX,
                           g_FieldFxSpriteClutY + g_FieldFxScratch->clutY);
    gte_stsxy2(&packet->x3);
    packet->tag.address = STRIP_OT(g_FieldFxScratch->depth + sprite->depth)->address;
    link.tag = &packet->tag;
    STRIP_OT(g_FieldFxScratch->depth + sprite->depth)->address = link.word;
    g_DrawPacketOffset += sizeof(FieldStripPacket);
}

/* Project a camera-facing billboard and link it into the depth ordering table. */
void FieldFx_DrawBillboard(FieldBillboard *board)
{
    FieldStripPacket *packet;
    int width;
    int height;
    int halfWidth;
    int halfHeight;
    int distance;
    int projectedWidth;
    int projectedHeight;
    FieldStripLink link;

    g_FieldFxBillboardScratch = FIELD_BILLBOARD_SCRATCH;
    g_FieldFxBillboardScratch->v = board->cell & 0xF0;
    g_FieldFxBillboardScratch->u = (board->cell - g_FieldFxBillboardScratch->v) << 4;
    g_FieldFxBillboardScratch->clutX = board->clut << 4;
    g_FieldFxBillboardScratch->clutY = board->clut >> 4;
    width = (g_FieldFxSpriteCellUMax * board->scaleX) >> 12;
    height = (g_FieldFxSpriteCellVExtent * board->scaleY) >> 12;
    if (board->brightness != 0x80) {
        g_FieldFxBillboardScratch->channel = board->rgb[0];
        g_FieldFxBillboardScratch->channel *= board->brightness;
        if (g_FieldFxBillboardScratch->channel > 0x7FFF) {
            g_FieldFxBillboardScratch->channel = 0x7FFF;
        }
        g_FieldFxBillboardScratch->channel >>= 7;
        g_FieldFxBillboardScratch->rgb[0] = g_FieldFxBillboardScratch->channel;
        g_FieldFxBillboardScratch->channel = board->rgb[1];
        g_FieldFxBillboardScratch->channel *= board->brightness;
        if (g_FieldFxBillboardScratch->channel > 0x7FFF) {
            g_FieldFxBillboardScratch->channel = 0x7FFF;
        }
        g_FieldFxBillboardScratch->channel >>= 7;
        g_FieldFxBillboardScratch->rgb[1] = g_FieldFxBillboardScratch->channel;
        g_FieldFxBillboardScratch->channel = board->rgb[2];
        g_FieldFxBillboardScratch->channel *= board->brightness;
        if (g_FieldFxBillboardScratch->channel > 0x7FFF) {
            g_FieldFxBillboardScratch->channel = 0x7FFF;
        }
        g_FieldFxBillboardScratch->channel >>= 7;
        g_FieldFxBillboardScratch->rgb[2] = g_FieldFxBillboardScratch->channel;
    } else {
        g_FieldFxBillboardScratch->rgb[0] = board->rgb[0];
        g_FieldFxBillboardScratch->rgb[1] = board->rgb[1];
        g_FieldFxBillboardScratch->rgb[2] = board->rgb[2];
    }
    {
        GteMatrix **slot;
        slot = &g_ViewMatrixSlot.value;
        gte_ldrotmatrix((const GteMatrixWords *)*slot);
        gte_ldtransmatrix((const GteMatrixWords *)*slot);
    }
    g_FieldFxBillboardScratch->depth = RotTransPers(&board->position, &g_FieldFxBillboardScratch->screen.word,
                                     &g_FieldFxBillboardScratch->p, &g_FieldFxBillboardScratch->flag);
    g_FieldFxBillboardScratch->centerX = g_FieldFxBillboardScratch->screen.word;
    g_FieldFxBillboardScratch->centerY = g_FieldFxBillboardScratch->screen.xy.y;
    distance = g_GeomScroll.state.position.projectionDistance[0];
    projectedWidth = (width << 5) * distance / (g_FieldFxBillboardScratch->depth * 4 + distance);
    projectedHeight =
        (height << 5) * distance / (g_FieldFxBillboardScratch->depth * 4 + distance);
    halfWidth = projectedWidth >> 1;
    halfHeight = projectedHeight >> 1;
    packet = (FieldStripPacket *)(g_RenderAnimBufferBase[g_ActiveDrawSlot] + g_DrawPacketOffset);
    packet->tag.length = 9;
    packet->code = 0x2C;
    /* PSY-Q setSemiTrans(packet, g_FieldFxSpriteSemiTrans). */
    if (g_FieldFxSpriteSemiTrans) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    if (board->angle != 0) {
        g_FieldFxBillboardScratch->corner[0].x = -halfWidth;
        g_FieldFxBillboardScratch->corner[0].y = -halfHeight;
        g_FieldFxBillboardScratch->corner[1].x = halfWidth;
        g_FieldFxBillboardScratch->corner[1].y = -halfHeight;
        g_FieldFxBillboardScratch->corner[2].x = -halfWidth;
        g_FieldFxBillboardScratch->corner[2].y = halfHeight;
        g_FieldFxBillboardScratch->corner[3].x = halfWidth;
        g_FieldFxBillboardScratch->corner[3].y = halfHeight;
        g_FieldFxBillboardScratch->rotation.m[2][2] = 0x1000;
        g_FieldFxBillboardScratch->rotation.m[1][1] = 0x1000;
        g_FieldFxBillboardScratch->rotation.m[0][0] = 0x1000;
        g_FieldFxBillboardScratch->rotation.t[2] = 0;
        g_FieldFxBillboardScratch->rotation.t[1] = 0;
        g_FieldFxBillboardScratch->rotation.t[0] = 0;
        g_FieldFxBillboardScratch->rotation.m[2][1] = 0;
        g_FieldFxBillboardScratch->rotation.m[2][0] = 0;
        g_FieldFxBillboardScratch->rotation.m[1][2] = 0;
        g_FieldFxBillboardScratch->rotation.m[1][0] = 0;
        g_FieldFxBillboardScratch->rotation.m[0][2] = 0;
        g_FieldFxBillboardScratch->rotation.m[0][1] = 0;
        RotMatrixZ(board->angle, &g_FieldFxBillboardScratch->rotation);
        ApplyMatrixSV(&g_FieldFxBillboardScratch->rotation, &g_FieldFxBillboardScratch->corner[0],
                      &g_FieldFxBillboardScratch->turned[0]);
        ApplyMatrixSV(&g_FieldFxBillboardScratch->rotation, &g_FieldFxBillboardScratch->corner[1],
                      &g_FieldFxBillboardScratch->turned[1]);
        ApplyMatrixSV(&g_FieldFxBillboardScratch->rotation, &g_FieldFxBillboardScratch->corner[2],
                      &g_FieldFxBillboardScratch->turned[2]);
        ApplyMatrixSV(&g_FieldFxBillboardScratch->rotation, &g_FieldFxBillboardScratch->corner[3],
                      &g_FieldFxBillboardScratch->turned[3]);
        packet->x0 = g_FieldFxBillboardScratch->centerX + g_FieldFxBillboardScratch->turned[0].x;
        packet->y0 = g_FieldFxBillboardScratch->centerY + g_FieldFxBillboardScratch->turned[0].y;
        packet->x1 = g_FieldFxBillboardScratch->centerX + g_FieldFxBillboardScratch->turned[1].x;
        packet->y1 = g_FieldFxBillboardScratch->centerY + g_FieldFxBillboardScratch->turned[1].y;
        packet->x2 = g_FieldFxBillboardScratch->centerX + g_FieldFxBillboardScratch->turned[2].x;
        packet->y2 = g_FieldFxBillboardScratch->centerY + g_FieldFxBillboardScratch->turned[2].y;
        packet->x3 = g_FieldFxBillboardScratch->centerX + g_FieldFxBillboardScratch->turned[3].x;
        packet->y3 = g_FieldFxBillboardScratch->centerY + g_FieldFxBillboardScratch->turned[3].y;
    } else {
        packet->x0 = g_FieldFxBillboardScratch->centerX - halfWidth;
        packet->y0 = g_FieldFxBillboardScratch->centerY - halfHeight;
        packet->x1 = g_FieldFxBillboardScratch->centerX + halfWidth;
        packet->y1 = g_FieldFxBillboardScratch->centerY - halfHeight;
        packet->x2 = g_FieldFxBillboardScratch->centerX - halfWidth;
        packet->y2 = g_FieldFxBillboardScratch->centerY + halfHeight;
        packet->x3 = g_FieldFxBillboardScratch->centerX + halfWidth;
        packet->y3 = g_FieldFxBillboardScratch->centerY + halfHeight;
    }
    packet->r0 = g_FieldFxBillboardScratch->rgb[0];
    packet->g0 = g_FieldFxBillboardScratch->rgb[1];
    packet->b0 = g_FieldFxBillboardScratch->rgb[2];
    packet->tpage = g_FieldFxSpriteTpageValue;
    packet->clut = GetClut(g_FieldFxSpriteClutX + g_FieldFxBillboardScratch->clutX,
                           g_FieldFxSpriteClutY + g_FieldFxBillboardScratch->clutY);
    packet->u0 = g_FieldFxBillboardScratch->u + 1;
    packet->v0 = g_FieldFxBillboardScratch->v + 1;
    packet->u1 = g_FieldFxBillboardScratch->u + g_FieldFxSpriteCellUMax;
    packet->v1 = g_FieldFxBillboardScratch->v + 1;
    packet->u2 = g_FieldFxBillboardScratch->u + 1;
    packet->v2 = g_FieldFxBillboardScratch->v + g_FieldFxSpriteCellVExtent;
    packet->u3 = g_FieldFxBillboardScratch->u + g_FieldFxSpriteCellUMax;
    packet->v3 = g_FieldFxBillboardScratch->v + g_FieldFxSpriteCellVExtent;
    packet->tag.address = STRIP_OT(g_FieldFxBillboardScratch->depth + board->depth)->address;
    link.tag = &packet->tag;
    STRIP_OT(g_FieldFxBillboardScratch->depth + board->depth)->address = link.word;
    g_DrawPacketOffset += sizeof(FieldStripPacket);
}

/* Compose/project a glow quad and link it into the depth ordering table.
 * The GTE operands are the camera slot and the corner vectors themselves:
 * reload loads them into t1..t3 in turn, as retail does. */
void FieldFx_DrawGlowSprite(FieldGlowSprite *sprite, GteMatrix *placement, u8 mode)
{
    s16 *xy0;
    s16 *xy1;
    s16 *xy2;
    FieldStripPacket *packet;
    GteShortVector position;
    int swap[4];
    s32 flag;
    FieldStripLink link;

    packet = (FieldStripPacket *)(g_RenderAnimBufferBase[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_FieldFxScratch = FIELDFX_SCRATCH;
    if (mode == 0) {
        /* Compose camera rotation with each placement column. */
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldclmv(placement);
        gte_rtir();
        gte_stclmv(&g_FieldFxScratch->matrix);
        gte_ldclmv((u16 *)placement + 1);
        gte_rtir();
        gte_stclmv((u16 *)&g_FieldFxScratch->matrix + 1);
        gte_ldclmv((u16 *)placement + 2);
        gte_rtir();
        gte_stclmv((u16 *)&g_FieldFxScratch->matrix + 2);
        /* Transform placement translation with the camera matrix. */
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        gte_ldlv0(placement->t);
        gte_rt();
        gte_stlvl(g_FieldFxScratch->matrix.t);
        gte_ldrotmatrix(&g_FieldFxScratch->matrix);
        gte_ldtransmatrix(&g_FieldFxScratch->matrix);
    } else {
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        gte_ldrotmatrix((const GteMatrixWords *)placement);
        gte_ldtransmatrix((const GteMatrixWords *)placement);
    }
    FieldFx_ScaleColor((s16)sprite->depth, &sprite->r, &packet->r0);
    gte_ldv3(g_FieldFxCellQuad, &g_FieldFxCellQuad[1], &g_FieldFxCellQuad[2]);
    gte_rtpt_padded();
    g_FieldFxScratch->v = g_FieldFxSpriteCellVBase + (sprite->cell & 0xF0);
    g_FieldFxScratch->u = (sprite->cell - g_FieldFxScratch->v) << 4;
    g_FieldFxScratch->clutX = sprite->clut << 4;
    g_FieldFxScratch->clutY = sprite->clut >> 4;
    packet->tag.length = 9;
    packet->code = 0x2C;
    packet->u0 = g_FieldFxScratch->u;
    packet->v0 = g_FieldFxScratch->v;
    packet->u1 = g_FieldFxScratch->u + g_FieldFxSpriteCellUMax;
    packet->v1 = g_FieldFxScratch->v;
    packet->u2 = g_FieldFxScratch->u;
    packet->v2 = g_FieldFxScratch->v + g_FieldFxSpriteCellVExtent;
    packet->u3 = g_FieldFxScratch->u + g_FieldFxSpriteCellUMax;
    packet->v3 = g_FieldFxScratch->v + g_FieldFxSpriteCellVExtent;
    if (sprite->flip & 1) {
        swap[0] = packet->u0;
        swap[1] = packet->u1;
        swap[2] = packet->u2;
        swap[3] = packet->u3;
        packet->u0 = swap[1];
        packet->u1 = swap[0];
        packet->u2 = swap[3];
        packet->u3 = swap[2];
    }
    if (sprite->flip & 2) {
        swap[0] = packet->v0;
        swap[1] = packet->v1;
        swap[2] = packet->v2;
        swap[3] = packet->v3;
        packet->v0 = swap[2];
        packet->v1 = swap[3];
        packet->v2 = swap[0];
        packet->v3 = swap[1];
    }
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_avsz3_command();
    /* PSY-Q setSemiTrans(packet, g_FieldFxSpriteSemiTrans). */
    if (g_FieldFxSpriteSemiTrans) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&g_FieldFxScratch->depth);
    g_FieldFxScratch->depth += sprite->offset;
    if (g_FieldFxScratch->depth - 1 < 0xFFFU) {
        xy0 = &packet->x0;
        xy1 = &packet->x1;
        xy2 = &packet->x2;
        gte_stsxy3(xy0, xy1, xy2);
        gte_ldv0(&g_FieldFxCellQuad[3]);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        packet->tpage = g_FieldFxSpriteTpageValue;
        packet->clut = GetClut(g_FieldFxSpriteClutX + g_FieldFxScratch->clutX,
                               g_FieldFxSpriteClutY + g_FieldFxScratch->clutY);
        gte_stsxy2(&packet->x3);
        packet->tag.address = STRIP_OT(g_FieldFxScratch->depth)->address;
        link.tag = &packet->tag;
        STRIP_OT(g_FieldFxScratch->depth)->address = link.word;
        g_DrawPacketOffset += sizeof(FieldStripPacket);
    }
}

/* Draw a quad with four independently shaded corners. */
void FieldFx_DrawShadedQuad(FieldShadedQuadColors *colors, GteMatrix *placement,
                   u8 mode) {
    s16 *xy0;
    s16 *xy1;
    s16 *xy2;
    FieldShadedQuadPacket *packet;
    GteShortVector position;
    s32 flag;
    FieldShadedQuadLink link;

    packet = (FieldShadedQuadPacket *)(g_RenderAnimBufferBase[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_FieldFxScratch = FIELDFX_SCRATCH;
    if (mode == 0) {
        /* Compose camera rotation with each placement column. */
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldclmv(placement);
        gte_rtir();
        gte_stclmv(&g_FieldFxScratch->matrix);
        gte_ldclmv((u16 *)placement + 1);
        gte_rtir();
        gte_stclmv((u16 *)&g_FieldFxScratch->matrix + 1);
        gte_ldclmv((u16 *)placement + 2);
        gte_rtir();
        gte_stclmv((u16 *)&g_FieldFxScratch->matrix + 2);
        /* Transform placement translation with the camera matrix. */
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        gte_ldlv0(placement->t);
        gte_rt();
        gte_stlvl(g_FieldFxScratch->matrix.t);
        gte_ldrotmatrix(&g_FieldFxScratch->matrix);
        gte_ldtransmatrix(&g_FieldFxScratch->matrix);
    } else {
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        gte_ldrotmatrix((const GteMatrixWords *)placement);
        gte_ldtransmatrix((const GteMatrixWords *)placement);
    }
    FieldFx_ScaleColor(colors->brightness, colors->rgb[0], &packet->r0);
    FieldFx_ScaleColor(colors->brightness, colors->rgb[1], &packet->r1);
    FieldFx_ScaleColor(colors->brightness, colors->rgb[2], &packet->r2);
    FieldFx_ScaleColor(colors->brightness, colors->rgb[3], &packet->r3);
    gte_ldv3(g_FieldFxCellQuad, &g_FieldFxCellQuad[1], &g_FieldFxCellQuad[2]);
    gte_rtpt_padded();
    packet->tag.length = 8;
    packet->code = 0x38;
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_avsz3_command();
    /* PSY-Q setSemiTrans(packet, g_FieldFxSpriteSemiTrans). */
    if (g_FieldFxSpriteSemiTrans) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&g_FieldFxScratch->depth);
    g_FieldFxScratch->depth += colors->depth;
    if (g_FieldFxScratch->depth - 1 < 0xFFFU) {
        xy0 = &packet->x0;
        xy1 = &packet->x1;
        xy2 = &packet->x2;
        gte_stsxy3(xy0, xy1, xy2);
        gte_ldv0(&g_FieldFxCellQuad[3]);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        gte_stsxy2(&packet->x3);
        packet->tag.address = SHADED_QUAD_OT(g_FieldFxScratch->depth)->address;
        link.tag = &packet->tag;
        SHADED_QUAD_OT(g_FieldFxScratch->depth)->address = link.word;
        g_DrawPacketOffset += sizeof(FieldShadedQuadPacket);
    }
}

/* Lay out the inner and outer vertices of an annulus. */
void FieldFx_BuildRingGeometry(FieldRingGeometry *data) {
    int angle;
    u32 i;
    int step;
    GteShortVector *verts;

    angle = 0;
    step = 0x1000 / data->mode;
    verts = data->source;

    for (i = 0; i < data->mode; i++, verts++) {
        verts->x = (rsin(angle) * data->extent1) >> 12;
        verts->y = (rcos(angle) * data->extent1) >> 12;
        verts->z = 0;
        angle += step;
    }

    angle = 0;
    for (i = 0; i < data->mode; i++, verts++) {
        verts->x = (rsin(angle) * data->extent0) >> 12;
        verts->y = (rcos(angle) * data->extent0) >> 12;
        verts->z = 0;
        angle += step;
    }
}

void FieldFx_DrawShadedRing(FieldShadedRing *ring, GteMatrix *placement, u8 mode)
{
    FieldRingPacket *packet;
    char *drawMode;
    u8 outer[4];
    u8 inner[4];
    GteShortVector position;
    s32 flag;
    s32 p;
    GteShortVector *points;
    u32 i;
    u32 next;
    u16 a;
    u16 c;
    u16 d;
    FieldRingLink link;

    g_FieldFxScratch = FIELDFX_SCRATCH;
    if (mode == 0) {
        /* Compose camera rotation with each placement column. */
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldclmv(placement);
        gte_rtir();
        gte_stclmv(&g_FieldFxScratch->matrix);
        gte_ldclmv((u16 *)placement + 1);
        gte_rtir();
        gte_stclmv((u16 *)&g_FieldFxScratch->matrix + 1);
        gte_ldclmv((u16 *)placement + 2);
        gte_rtir();
        gte_stclmv((u16 *)&g_FieldFxScratch->matrix + 2);
        /* Transform placement translation with the camera matrix. */
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        gte_ldlv0(placement->t);
        gte_rt();
        gte_stlvl(g_FieldFxScratch->matrix.t);
        gte_ldrotmatrix(&g_FieldFxScratch->matrix);
        gte_ldtransmatrix(&g_FieldFxScratch->matrix);
    } else {
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        gte_ldrotmatrix((const GteMatrixWords *)placement);
        gte_ldtransmatrix((const GteMatrixWords *)placement);
    }
    FieldFx_ScaleColor(ring->intensity, ring->color1, outer);
    FieldFx_ScaleColor(ring->intensity, ring->color0, inner);
    points = ring->source;
    for (i = 0; i < ring->mode; i++) {
        packet = (FieldRingPacket *)(g_RenderAnimBufferBase[g_ActiveDrawSlot] + g_DrawPacketOffset);
        g_DrawPacketOffset += sizeof(FieldRingPacket);
        drawMode = g_RenderAnimBufferBase[g_ActiveDrawSlot] + g_DrawPacketOffset;
        g_DrawPacketOffset += 8;
        SetDrawTPage(drawMode, 0, 0, (g_FieldFxSpriteBlendRate & 3) << 5);
        next = (i + 1) % ring->mode;
        a = i;
        c = ring->mode + i;
        d = ring->mode + next;
        g_FieldFxScratch->depth = RotTransPers4(&points[a], &points[next], &points[c],
                                          &points[d], &packet->x0, &packet->x1,
                                          &packet->x2, &packet->x3, &p, &flag);
        /* Word copies through plain pointers (not struct fields), so the
         * g_FieldFxSpriteSemiTrans load below stays after them. */
        *(u32 *)&packet->c0 = *(u32 *)outer;
        *(u32 *)&packet->c1 = *(u32 *)outer;
        *(u32 *)&packet->c2 = *(u32 *)inner;
        *(u32 *)&packet->c3 = *(u32 *)inner;
        packet->tag.length = 8;
        packet->c0.rgb[3] = 0x38;
        /* PSY-Q setSemiTrans(packet, g_FieldFxSpriteSemiTrans). */
        if (g_FieldFxSpriteSemiTrans) {
            packet->c0.rgb[3] = packet->c0.rgb[3] | 2;
        } else {
            packet->c0.rgb[3] = packet->c0.rgb[3] & ~2;
        }
        packet->tag.address = RING_OT(g_FieldFxScratch->depth + ring->offset)->address;
        link.tag = &packet->tag;
        RING_OT(g_FieldFxScratch->depth + ring->offset)->address = link.word;
        link.bytes = drawMode;
        link.tag->address = RING_OT(g_FieldFxScratch->depth + ring->offset)->address;
        RING_OT(g_FieldFxScratch->depth + ring->offset)->address = link.word;
    }
}

void FieldFx_BuildParticleChain(FieldChainRecord *record)
{
    GteShortVector tailIn;
    GteShortVector tail;
    GteShortVector headIn;
    GteShortVector head;
    GteMatrix matrix;
    GteShortVector angles;
    GteMatrix rotation;
    GteVector previous;
    GteVector toPlayer;
    GteVector cross;
    FieldChainLink *link;
    unsigned int i;
    int run;
    int turn;

    link = record->links;
    memset(&tail, 0, sizeof(tail));
    tail.x = -record->length;
    tail.y = 0;
    tail.z = 0;
    tailIn = tail;
    memset(&head, 0, sizeof(head));
    head.x = record->length;
    head.y = 0;
    head.z = 0;
    headIn = head;
    matrix = record->matrix;
    record->field0E = 0;
    for (i = 0; i < record->count; i++, link++) {
        ApplyMatrixSV(&matrix, &tailIn, &tail);
        ApplyMatrixSV(&matrix, &headIn, &head);
        link->edgeA.x = head.x + matrix.t[0];
        link->edgeA.y = head.y + matrix.t[1];
        link->edgeA.z = head.z + matrix.t[2];
        link->edgeB.x = tail.x + matrix.t[0];
        link->edgeB.y = tail.y + matrix.t[1];
        link->edgeB.z = tail.z + matrix.t[2];
        if (i >= 2)
            record->bend = 1;
        else
            record->bend = 0;
        if (record->bend == 1) {
            previous.x = link[-1].matrix.t[0] - link[-2].matrix.t[0];
            previous.y = 0;
            previous.z = link[-1].matrix.t[2] - link[-2].matrix.t[2];
            toPlayer.x = ((RoomSparkBattleEntity *)D_8009CD10)->x.part.integer - link[-2].matrix.t[0];
            toPlayer.y = 0;
            toPlayer.z = ((RoomSparkBattleEntity *)D_8009CD10)->z.part.integer - link[-2].matrix.t[2];
            OuterProduct0(&previous, &toPlayer, &cross);
            run = SquareRoot0(previous.x * previous.x + previous.z * previous.z);
            ratan2(run, SquareRoot0(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z));
            if (cross.y > 0)
                turn = rand() % 512 + 0x40;
            else
                turn = -(rand() % 512 + 0x40);
            angles.y = turn;
            angles.x = 0;
            angles.z = 0;
        } else {
            angles.x = link->tiltX;
            angles.y = link->tiltY;
            angles.z = 0;
        }
        if (i >= 6) {
            angles.x = 0;
            angles.y = 0;
            angles.z = 0;
        }
        RotMatrix(&angles, &rotation);
        rotation.t[0] = 0;
        rotation.t[1] = 0;
        rotation.t[2] = -record->depth;
        gte_CompMatrix(&matrix, &rotation, &matrix);
        link->matrix = matrix;
    }
}

void FieldFx_DrawTexturedStrip(FieldTexturedStrip *strip)
{
    FieldStripNode *node;
    FieldStripPacket *packet;
    GteShortVector quad[4];
    s32 p;
    s32 flag;
    u32 i;
    u8 cell;
    int step;
    u32 depth;
    FieldStripLink link;

    node = strip->nodes;
    i = 0;
    g_FieldFxScratch = FIELDFX_SCRATCH;
    packet = (FieldStripPacket *)(g_RenderAnimBufferBase[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_FieldFxScratch->clutX = strip->clut << 4;
    g_FieldFxScratch->clutY = strip->clut >> 4;
    while (i < strip->count - 1) {
        cell = strip->cell;
        step = node->cellStep;
        g_FieldFxScratch->v = cell & 0xF0;
        g_FieldFxScratch->u = (strip->cell - g_FieldFxScratch->v) << 4;
        cell += step;
        g_FieldFxScratch->v = cell & 0xF0;
        g_FieldFxScratch->u = (cell - g_FieldFxScratch->v) << 4;
        quad[0] = node->edgeA;
        quad[1] = node->edgeB;
        quad[2] = node[1].edgeA;
        quad[3] = node[1].edgeB;
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        depth = RotTransPers4(&quad[0], &quad[1], &quad[2], &quad[3],
                              &packet->x0, &packet->x1, &packet->x2,
                              &packet->x3, &p, &flag);
        FieldFx_ScaleColor(node->brightness, node->rgb, &packet->r0);
        packet->u0 = g_FieldFxScratch->u;
        packet->v0 = g_FieldFxScratch->v;
        packet->u1 = g_FieldFxScratch->u + g_FieldFxSpriteCellUMax;
        packet->v1 = g_FieldFxScratch->v;
        packet->u2 = g_FieldFxScratch->u;
        packet->v2 = g_FieldFxScratch->v + g_FieldFxSpriteCellVExtent;
        packet->u3 = g_FieldFxScratch->u + g_FieldFxSpriteCellUMax;
        packet->v3 = g_FieldFxScratch->v + g_FieldFxSpriteCellVExtent;
        packet->tpage = g_FieldFxSpriteTpageValue;
        packet->clut = GetClut(g_FieldFxSpriteClutX + g_FieldFxScratch->clutX,
                               g_FieldFxSpriteClutY + g_FieldFxScratch->clutY);
        packet->tag.length = 9;
        packet->code = 0x2C;
        /* PSY-Q setSemiTrans(packet, g_FieldFxSpriteSemiTrans). */
        if (g_FieldFxSpriteSemiTrans) {
            packet->code = packet->code | 2;
        } else {
            packet->code = packet->code & ~2;
        }
        if (depth != 0 && depth < 0x1000 && node->visible) {
            packet->tag.address = STRIP_OT(depth)->address;
            link.tag = &packet->tag;
            STRIP_OT(depth)->address = link.word;
        }
        i++;
        node++;
        packet++;
        g_DrawPacketOffset += sizeof(FieldStripPacket);
    }
}
