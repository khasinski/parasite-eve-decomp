/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/psyq_bios.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_oriented_sprite.h"
#include "pe1/field_billboard.h"
#include "pe1/field_glow_layers.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_textured_strip.h"
#include "pe1/field_shaded_ring.h"
#include "pe1/field_particle_chain.h"

/* Field engine sprite rendering (0x800C2EAC..0x800C5EB0): the draw state
 * (texture page, CLUT origin, blend mode, cell quad, colour ramps) and the
 * sprite draws that use it: oriented sprite, billboard, glow sprite, shaded
 * quad, shaded ring, textured strip and the particle chain. They share the
 * draw state and scratch pointers private to this range (main_tu_evidence
 * G1390). */

extern char D_800C2110[];

void func_800C2EAC(u8 mode) {
    if (mode == 0) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D7;
        D_800F3422 = 0;
    }
    if (mode == 1) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x160;
        D_800F341E = 0x1DB;
        D_800F341C = 0;
        D_800F3422 = 0x60;
    }
    if (mode == 2) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D6;
        D_800F3422 = 0;
    }
    if (mode == 3) {
        D_800F3424 = 0x380;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1C8;
        D_800F3422 = 0;
    }

    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}

void func_800C2FF0(int width, int height) {
    int widthMinus;
    int heightMinus;
    int x;
    int y;
    widthMinus = width - 1;
    heightMinus = height - 1;
    asm("" : : "r"(widthMinus), "r"(heightMinus));

    D_800F345C = width;
    x = (width & 0xFF) << 4;
    D_800F345D = height;
    y = (height & 0xFF) << 4;

    D_800F3310.x = -x;
    D_800F3310.y = -y;
    D_800F3310.z = 0;
    D_800F3318.x = x;
    D_800F3318.y = -y;
    D_800F3318.z = 0;
    D_800F3320.x = -x;
    D_800F3320.y = y;
    D_800F3320.z = 0;
    D_800F3328.x = x;
    D_800F3328.y = y;
    D_800F3328.z = 0;

    D_800F345C = widthMinus;
    D_800F345D = heightMinus;
}

void func_800C3098(int mode) {
    mode = (short)mode;

    if (mode == 0x10) {
        goto mode16;
    }
    if (mode == 0x100) {
        goto mode256;
    }
    goto badMode;

mode16:
    D_800F33AC = 0;
    goto done;

mode256:
    D_800F33AC = 1;
    goto done;

badMode:
    printf(D_800C2110);

done:
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}

void func_800C3134(u8 *table, u32 step, u8 *out) {
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

void func_800C3238(u8 mode) {
    D_800F33B8 = mode;
    switch (mode) {
    case 0:
        D_800F337A = 0;
        D_800E224C = 0;
        break;
    case 1:
        D_800F337A = 1;
        D_800E224C = 0;
        break;
    case 2:
        D_800F337A = 1;
        D_800E224C = 1;
        break;
    case 3:
        D_800F337A = 1;
        D_800E224C = 2;
        break;
    case 4:
        D_800F337A = 1;
        D_800E224C = 3;
        break;
    }
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}

void func_800C3324(FieldOrientedSprite *sprite)
{
    FieldStripPacket *packet;
    FieldStripLink link;

    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    D_800F33B4->v = sprite->cell & 0xF0;
    D_800F33B4->u = (sprite->cell - D_800F33B4->v) << 4;
    D_800F33B4->clutX = sprite->clut << 4;
    D_800F33B4->clutY = sprite->clut >> 4;
    if (sprite->brightness != 0x80) {
        D_800F33B4->channel = sprite->rgb[0];
        D_800F33B4->channel *= sprite->brightness;
        if (D_800F33B4->channel > 0x7FFF) {
            D_800F33B4->channel = 0x7FFF;
        }
        D_800F33B4->channel >>= 7;
        packet->r0 = D_800F33B4->channel;
        D_800F33B4->channel = sprite->rgb[1];
        D_800F33B4->channel *= sprite->brightness;
        if (D_800F33B4->channel > 0x7FFF) {
            D_800F33B4->channel = 0x7FFF;
        }
        D_800F33B4->channel >>= 7;
        packet->g0 = D_800F33B4->channel;
        D_800F33B4->channel = sprite->rgb[2];
        D_800F33B4->channel *= sprite->brightness;
        if (D_800F33B4->channel > 0x7FFF) {
            D_800F33B4->channel = 0x7FFF;
        }
        D_800F33B4->channel >>= 7;
        packet->b0 = D_800F33B4->channel;
    } else {
        packet->r0 = sprite->rgb[0];
        packet->g0 = sprite->rgb[1];
        packet->b0 = sprite->rgb[2];
    }
    RotMatrixYXZ(&sprite->rotation, &D_800F33B4->local);
    ScaleMatrix(&D_800F33B4->local, &sprite->scale);
    D_800F33B4->local.t[0] = 0;
    D_800F33B4->local.t[1] = 0;
    D_800F33B4->local.t[2] = 0;
    D_800F33B4->matrix = *D_800BCFA4.value;
    D_800F33B4->matrix.t[0] += (D_800F33B4->matrix.m[0][2] * sprite->position.z +
                                D_800F33B4->matrix.m[0][1] * sprite->position.y +
                                D_800F33B4->matrix.m[0][0] * sprite->position.x) / 4096;
    D_800F33B4->matrix.t[1] += (D_800F33B4->matrix.m[1][2] * sprite->position.z +
                                D_800F33B4->matrix.m[1][1] * sprite->position.y +
                                D_800F33B4->matrix.m[1][0] * sprite->position.x) / 4096;
    D_800F33B4->matrix.t[2] += (D_800F33B4->matrix.m[2][2] * sprite->position.z +
                                D_800F33B4->matrix.m[2][1] * sprite->position.y +
                                D_800F33B4->matrix.m[2][0] * sprite->position.x) / 4096;
    gte_CompMatrix(&D_800F33B4->matrix, &D_800F33B4->local, &D_800F33B4->matrix);
    gte_ldrotmatrix(&D_800F33B4->matrix);
    gte_ldtransmatrix(&D_800F33B4->matrix);
    gte_ldv3(&D_800F3310, &D_800F3318, &D_800F3320);
    gte_rtpt_padded();
    packet->tag.length = 9;
    packet->code = 0x2C;
    packet->u0 = D_800F33B4->u;
    packet->v0 = D_800F33B4->v;
    packet->u1 = D_800F33B4->u + D_800F345C;
    packet->v1 = D_800F33B4->v;
    packet->u2 = D_800F33B4->u;
    packet->v2 = D_800F33B4->v + D_800F345D;
    packet->u3 = D_800F33B4->u + D_800F345C;
    packet->v3 = D_800F33B4->v + D_800F345D;
    gte_avsz3_padded();
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&D_800F33B4->depth);
    gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
    gte_ldv0(&D_800F3328);
    gte_rtps();
    packet->tpage = D_800E27AC;
    packet->clut = GetClut(D_800F341C + D_800F33B4->clutX,
                           D_800F341E + D_800F33B4->clutY);
    gte_stsxy2(&packet->x3);
    packet->tag.address = STRIP_OT(D_800F33B4->depth + sprite->depth)->address;
    link.tag = &packet->tag;
    STRIP_OT(D_800F33B4->depth + sprite->depth)->address = link.word;
    D_8009CDD8 += sizeof(FieldStripPacket);
}

/* Matching debt: four register pins and one empty slot-address barrier.
 * Matrix loads are C; each GTE transfer uses its individual macro. */
void func_800C3B04(FieldBillboard *board)
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

    D_800E284C = FIELD_BILLBOARD_SCRATCH;
    D_800E284C->v = board->cell & 0xF0;
    D_800E284C->u = (board->cell - D_800E284C->v) << 4;
    D_800E284C->clutX = board->clut << 4;
    D_800E284C->clutY = board->clut >> 4;
    width = (D_800F345C * board->scaleX) >> 12;
    height = (D_800F345D * board->scaleY) >> 12;
    if (board->brightness != 0x80) {
        D_800E284C->channel = board->rgb[0];
        D_800E284C->channel *= board->brightness;
        if (D_800E284C->channel > 0x7FFF) {
            D_800E284C->channel = 0x7FFF;
        }
        D_800E284C->channel >>= 7;
        D_800E284C->rgb[0] = D_800E284C->channel;
        D_800E284C->channel = board->rgb[1];
        D_800E284C->channel *= board->brightness;
        if (D_800E284C->channel > 0x7FFF) {
            D_800E284C->channel = 0x7FFF;
        }
        D_800E284C->channel >>= 7;
        D_800E284C->rgb[1] = D_800E284C->channel;
        D_800E284C->channel = board->rgb[2];
        D_800E284C->channel *= board->brightness;
        if (D_800E284C->channel > 0x7FFF) {
            D_800E284C->channel = 0x7FFF;
        }
        D_800E284C->channel >>= 7;
        D_800E284C->rgb[2] = D_800E284C->channel;
    } else {
        D_800E284C->rgb[0] = board->rgb[0];
        D_800E284C->rgb[1] = board->rgb[1];
        D_800E284C->rgb[2] = board->rgb[2];
    }
    {
        GteMatrix **slot;
        /* Slot address stays in its own register; the loaded matrix is $t1. */
        register const GteMatrixWords *matrix asm("$9");
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        matrix = (const GteMatrixWords *)*slot;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
    }
    D_800E284C->depth = RotTransPers(&board->position, &D_800E284C->screen.word,
                                     &D_800E284C->p, &D_800E284C->flag);
    D_800E284C->centerX = D_800E284C->screen.word;
    D_800E284C->centerY = D_800E284C->screen.xy.y;
    distance = D_800BCFA8.distance[0];
    projectedWidth = (width << 5) * distance / (D_800E284C->depth * 4 + distance);
    projectedHeight =
        (height << 5) * distance / (D_800E284C->depth * 4 + distance);
    halfWidth = projectedWidth >> 1;
    halfHeight = projectedHeight >> 1;
    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    packet->tag.length = 9;
    packet->code = 0x2C;
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    if (board->angle != 0) {
        D_800E284C->corner[0].x = -halfWidth;
        D_800E284C->corner[0].y = -halfHeight;
        D_800E284C->corner[1].x = halfWidth;
        D_800E284C->corner[1].y = -halfHeight;
        D_800E284C->corner[2].x = -halfWidth;
        D_800E284C->corner[2].y = halfHeight;
        D_800E284C->corner[3].x = halfWidth;
        D_800E284C->corner[3].y = halfHeight;
        D_800E284C->rotation.m[2][2] = 0x1000;
        D_800E284C->rotation.m[1][1] = 0x1000;
        D_800E284C->rotation.m[0][0] = 0x1000;
        D_800E284C->rotation.t[2] = 0;
        D_800E284C->rotation.t[1] = 0;
        D_800E284C->rotation.t[0] = 0;
        D_800E284C->rotation.m[2][1] = 0;
        D_800E284C->rotation.m[2][0] = 0;
        D_800E284C->rotation.m[1][2] = 0;
        D_800E284C->rotation.m[1][0] = 0;
        D_800E284C->rotation.m[0][2] = 0;
        D_800E284C->rotation.m[0][1] = 0;
        RotMatrixZ(board->angle, &D_800E284C->rotation);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[0],
                      &D_800E284C->turned[0]);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[1],
                      &D_800E284C->turned[1]);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[2],
                      &D_800E284C->turned[2]);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[3],
                      &D_800E284C->turned[3]);
        packet->x0 = D_800E284C->centerX + D_800E284C->turned[0].x;
        packet->y0 = D_800E284C->centerY + D_800E284C->turned[0].y;
        packet->x1 = D_800E284C->centerX + D_800E284C->turned[1].x;
        packet->y1 = D_800E284C->centerY + D_800E284C->turned[1].y;
        packet->x2 = D_800E284C->centerX + D_800E284C->turned[2].x;
        packet->y2 = D_800E284C->centerY + D_800E284C->turned[2].y;
        packet->x3 = D_800E284C->centerX + D_800E284C->turned[3].x;
        packet->y3 = D_800E284C->centerY + D_800E284C->turned[3].y;
    } else {
        packet->x0 = D_800E284C->centerX - halfWidth;
        packet->y0 = D_800E284C->centerY - halfHeight;
        packet->x1 = D_800E284C->centerX + halfWidth;
        packet->y1 = D_800E284C->centerY - halfHeight;
        packet->x2 = D_800E284C->centerX - halfWidth;
        packet->y2 = D_800E284C->centerY + halfHeight;
        packet->x3 = D_800E284C->centerX + halfWidth;
        packet->y3 = D_800E284C->centerY + halfHeight;
    }
    packet->r0 = D_800E284C->rgb[0];
    packet->g0 = D_800E284C->rgb[1];
    packet->b0 = D_800E284C->rgb[2];
    packet->tpage = D_800E27AC;
    packet->clut = GetClut(D_800F341C + D_800E284C->clutX,
                           D_800F341E + D_800E284C->clutY);
    packet->u0 = D_800E284C->u + 1;
    packet->v0 = D_800E284C->v + 1;
    packet->u1 = D_800E284C->u + D_800F345C;
    packet->v1 = D_800E284C->v + 1;
    packet->u2 = D_800E284C->u + 1;
    packet->v2 = D_800E284C->v + D_800F345D;
    packet->u3 = D_800E284C->u + D_800F345C;
    packet->v3 = D_800E284C->v + D_800F345D;
    packet->tag.address = STRIP_OT(D_800E284C->depth + board->depth)->address;
    link.tag = &packet->tag;
    STRIP_OT(D_800E284C->depth + board->depth)->address = link.word;
    D_8009CDD8 += sizeof(FieldStripPacket);
}

/* Compose/project a glow quad and link it into the depth ordering table.
 * Matching debt: twelve register pins and ten empty address/memory barriers.
 * CPU loads, stores and packing are C; GTE commands/transfers and hazard nops
 * are individually wrapped. Columns stay expanded to preserve scheduling. */
void func_800C42A4(FieldGlowSprite *sprite, GteMatrix *placement, u8 mode)
{
    register const GteMatrixWords *cameraRot asm("$9");
    const GteMatrixWords *cameraTrans;
    const GteMatrixWords *composed;
    register const GteMatrixWords *camera asm("$11");
    const u16 *column;
    u16 *outColumn;
    register u16 *firstColumn asm("$10");
    s32 *outTranslation;
    const s32 *translation;
    GteMatrix **slot;
    register GteShortVector *v0 asm("$9");
    register GteShortVector *v1 asm("$10");
    register GteShortVector *v2 asm("$11");
    register s16 *xy0;
    register s16 *xy1;
    s16 *xy2;
    FieldStripPacket *packet;
    GteShortVector position;
    int swap[4];
    s32 flag;
    FieldStripLink link;

    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    if (mode == 0) {
        /* Compose camera rotation with each placement column. */
        cameraRot = (const GteMatrixWords *)D_800BCFA4.value;
        gte_ldrotmatrix(cameraRot);
        gte_ldclmv(placement);
        gte_rtir();
        firstColumn = (u16 *)&D_800F33B4->matrix;
                gte_stclmv(firstColumn);
        column = (const u16 *)placement + 1;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&D_800F33B4->matrix + 1;
                gte_stclmv(outColumn);
        column = (const u16 *)placement + 2;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&D_800F33B4->matrix + 2;
                gte_stclmv(outColumn);
        /* Transform placement translation with the camera matrix. */
        cameraTrans = (const GteMatrixWords *)D_800BCFA4.value;
        gte_ldtransmatrix(cameraTrans);
        translation = placement->t;
                gte_ldlv0(translation);
        gte_rt();
        outTranslation = D_800F33B4->matrix.t;
        gte_stlvl(outTranslation);
        composed = (const GteMatrixWords *)&D_800F33B4->matrix;
                gte_ldrotmatrix(composed);
        gte_ldtransmatrix(composed);
    } else {
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        camera = (const GteMatrixWords *)*slot;
        gte_ldrotmatrix(camera);
        gte_ldtransmatrix(camera);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        gte_ldrotmatrix((const GteMatrixWords *)placement);
        gte_ldtransmatrix((const GteMatrixWords *)placement);
    }
    func_800C608C((s16)sprite->depth, &sprite->r, &packet->r0);
    v0 = &D_800F3310;
    v1 = &D_800F3318;
    v2 = &D_800F3320;
    gte_lwc2_0_0(v0);
    gte_lwc2_1_4(v0);
    gte_lwc2_2_0(v1);
    gte_lwc2_3_4(v1);
    gte_lwc2_4_0(v2);
    gte_lwc2_5_4(v2);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtpt_command();
    D_800F33B4->v = D_800F3422 + (sprite->cell & 0xF0);
    D_800F33B4->u = (sprite->cell - D_800F33B4->v) << 4;
    D_800F33B4->clutX = sprite->clut << 4;
    D_800F33B4->clutY = sprite->clut >> 4;
    packet->tag.length = 9;
    packet->code = 0x2C;
    packet->u0 = D_800F33B4->u;
    packet->v0 = D_800F33B4->v;
    packet->u1 = D_800F33B4->u + D_800F345C;
    packet->v1 = D_800F33B4->v;
    packet->u2 = D_800F33B4->u;
    packet->v2 = D_800F33B4->v + D_800F345D;
    packet->u3 = D_800F33B4->u + D_800F345C;
    packet->v3 = D_800F33B4->v + D_800F345D;
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
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&D_800F33B4->depth);
    D_800F33B4->depth += sprite->offset;
    if (D_800F33B4->depth - 1 < 0xFFFU) {
        xy0 = &packet->x0;
        xy1 = &packet->x1;
        xy2 = &packet->x2;
        gte_stsxy3(xy0, xy1, xy2);
        v0 = &D_800F3328;
        gte_lwc2_0_0(v0);
        gte_lwc2_1_4(v0);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        packet->tpage = D_800E27AC;
        packet->clut = GetClut(D_800F341C + D_800F33B4->clutX,
                               D_800F341E + D_800F33B4->clutY);
        gte_stsxy2(&packet->x3);
        packet->tag.address = STRIP_OT(D_800F33B4->depth)->address;
        link.tag = &packet->tag;
        STRIP_OT(D_800F33B4->depth)->address = link.word;
        D_8009CDD8 += sizeof(FieldStripPacket);
    }
}

/* Draw a quad with four independently shaded corners.
 * Matching debt: twelve register pins and ten empty address/memory barriers.
 * CPU matrix loads, stores and packing are C. GTE instructions and hazard
 * nops have individual macros; columns stay expanded to preserve scheduling. */
void func_800C499C(FieldShadedQuadColors *colors, GteMatrix *placement,
                   u8 mode) {
    register const GteMatrixWords *cameraRot asm("$9");
    const GteMatrixWords *cameraTrans;
    const GteMatrixWords *composed;
    register const GteMatrixWords *camera asm("$11");
    const u16 *column;
    u16 *outColumn;
    register u16 *firstColumn asm("$10");
    s32 *outTranslation;
    const s32 *translation;
    GteMatrix **slot;
    register GteShortVector *v0 asm("$9");
    register GteShortVector *v1 asm("$10");
    register GteShortVector *v2 asm("$11");
    register s16 *xy0;
    register s16 *xy1;
    s16 *xy2;
    FieldShadedQuadPacket *packet;
    GteShortVector position;
    s32 flag;
    FieldShadedQuadLink link;

    packet = (FieldShadedQuadPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    if (mode == 0) {
        /* Compose camera rotation with each placement column. */
        cameraRot = (const GteMatrixWords *)D_800BCFA4.value;
        gte_ldrotmatrix(cameraRot);
        gte_ldclmv(placement);
        gte_rtir();
        firstColumn = (u16 *)&D_800F33B4->matrix;
                gte_stclmv(firstColumn);
        column = (const u16 *)placement + 1;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&D_800F33B4->matrix + 1;
                gte_stclmv(outColumn);
        column = (const u16 *)placement + 2;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&D_800F33B4->matrix + 2;
                gte_stclmv(outColumn);
        /* Transform placement translation with the camera matrix. */
        cameraTrans = (const GteMatrixWords *)D_800BCFA4.value;
        gte_ldtransmatrix(cameraTrans);
        translation = placement->t;
                gte_ldlv0(translation);
        gte_rt();
        outTranslation = D_800F33B4->matrix.t;
        gte_stlvl(outTranslation);
        composed = (const GteMatrixWords *)&D_800F33B4->matrix;
                gte_ldrotmatrix(composed);
        gte_ldtransmatrix(composed);
    } else {
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        camera = (const GteMatrixWords *)*slot;
        gte_ldrotmatrix(camera);
        gte_ldtransmatrix(camera);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        gte_ldrotmatrix((const GteMatrixWords *)placement);
        gte_ldtransmatrix((const GteMatrixWords *)placement);
    }
    func_800C608C(colors->brightness, colors->rgb[0], &packet->r0);
    func_800C608C(colors->brightness, colors->rgb[1], &packet->r1);
    func_800C608C(colors->brightness, colors->rgb[2], &packet->r2);
    func_800C608C(colors->brightness, colors->rgb[3], &packet->r3);
    v0 = &D_800F3310;
    v1 = &D_800F3318;
    v2 = &D_800F3320;
    gte_lwc2_0_0(v0);
    gte_lwc2_1_4(v0);
    gte_lwc2_2_0(v1);
    gte_lwc2_3_4(v1);
    gte_lwc2_4_0(v2);
    gte_lwc2_5_4(v2);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtpt_command();
    packet->tag.length = 8;
    packet->code = 0x38;
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_avsz3_command();
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&D_800F33B4->depth);
    D_800F33B4->depth += colors->depth;
    if (D_800F33B4->depth - 1 < 0xFFFU) {
        xy0 = &packet->x0;
        xy1 = &packet->x1;
        xy2 = &packet->x2;
        gte_stsxy3(xy0, xy1, xy2);
        v0 = &D_800F3328;
        gte_lwc2_0_0(v0);
        gte_lwc2_1_4(v0);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        gte_stsxy2(&packet->x3);
        packet->tag.address = SHADED_QUAD_OT(D_800F33B4->depth)->address;
        link.tag = &packet->tag;
        SHADED_QUAD_OT(D_800F33B4->depth)->address = link.word;
        D_8009CDD8 += sizeof(FieldShadedQuadPacket);
    }
}

int rsin(int angle);
int rcos(int angle);

/* Draw an annulus from paired inner/outer vertices.
 * Matching debt: seven register pins and eleven empty barriers. Matrix CPU
 * work is C; each GTE transfer/command and hazard nop is individually wrapped. */

void func_800C4E50(FieldRingGeometry *data) {
    register FieldRingGeometry *data_s3 asm("$19");
    int angle;
    u32 i;
    int step;
    char *verts;

    data_s3 = data;
    asm("" : "=r"(data_s3) : "0"(data_s3));
    angle = 0;
    step = 0x1000 / data_s3->mode;
    verts = (char *)data_s3->source;

    for (i = 0; i < data_s3->mode; i++, verts += 8) {
        *(s16 *)(verts + 0) = (rsin(angle) * data_s3->extent1) >> 12;
        *(s16 *)(verts + 2) = (rcos(angle) * data_s3->extent1) >> 12;
        *(s16 *)(verts + 4) = 0;
        angle += step;
    }

    angle = 0;
    for (i = 0; i < data_s3->mode; i++, verts += 8) {
        *(s16 *)(verts + 0) = (rsin(angle) * data_s3->extent0) >> 12;
        *(s16 *)(verts + 2) = (rcos(angle) * data_s3->extent0) >> 12;
        *(s16 *)(verts + 4) = 0;
        angle += step;
    }
}

void func_800C4FC4(FieldShadedRing *ring, GteMatrix *placement, u8 mode)
{
    const GteMatrixWords *cameraTrans;
    const GteMatrixWords *composed;
    register const GteMatrixWords *camera asm("$10");
    const u16 *column;
    u16 *outColumn;
    register u16 *firstColumn asm("$10");
    s32 *outTranslation;
    const s32 *translation;
    GteMatrix **slot;
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

    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    if (mode == 0) {
        /* Compose camera rotation with each placement column. */
        camera = (const GteMatrixWords *)D_800BCFA4.value;
        gte_ldrotmatrix(camera);
        gte_ldclmv(placement);
        gte_rtir();
        firstColumn = (u16 *)&D_800F33B4->matrix;
                gte_stclmv(firstColumn);
        column = (const u16 *)placement + 1;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&D_800F33B4->matrix + 1;
                gte_stclmv(outColumn);
        column = (const u16 *)placement + 2;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&D_800F33B4->matrix + 2;
                gte_stclmv(outColumn);
        /* Transform placement translation with the camera matrix. */
        cameraTrans = (const GteMatrixWords *)D_800BCFA4.value;
        gte_ldtransmatrix(cameraTrans);
        translation = placement->t;
                gte_ldlv0(translation);
        gte_rt();
        outTranslation = D_800F33B4->matrix.t;
        gte_stlvl(outTranslation);
        composed = (const GteMatrixWords *)&D_800F33B4->matrix;
                gte_ldrotmatrix(composed);
        gte_ldtransmatrix(composed);
    } else {
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        camera = (const GteMatrixWords *)*slot;
        gte_ldrotmatrix(camera);
        gte_ldtransmatrix(camera);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        gte_ldrotmatrix((const GteMatrixWords *)placement);
        gte_ldtransmatrix((const GteMatrixWords *)placement);
    }
    func_800C608C(ring->intensity, ring->color1, outer);
    func_800C608C(ring->intensity, ring->color0, inner);
    /* GCC 2.7.2 allocation debt: five input references keep ring in s1
     * without a hard pin that would reorder the prologue. Fewer references
     * change the allocation; this barrier emits no instructions. */
        points = ring->source;
    for (i = 0; i < ring->mode; i++) {
        packet = (FieldRingPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
        D_8009CDD8 += sizeof(FieldRingPacket);
        drawMode = D_800B0E58[D_8009CDDC] + D_8009CDD8;
        D_8009CDD8 += 8;
        SetDrawTPage(drawMode, 0, 0, (D_800E224C & 3) << 5);
        next = (i + 1) % ring->mode;
        a = i;
        c = ring->mode + i;
        d = ring->mode + next;
        D_800F33B4->depth = RotTransPers4(&points[a], &points[next], &points[c],
                                          &points[d], &packet->x0, &packet->x1,
                                          &packet->x2, &packet->x3, &p, &flag);
        /* Word copies through plain pointers (not struct fields), so the
         * D_800F337A load below stays after them. */
        *(u32 *)&packet->c0 = *(u32 *)outer;
        *(u32 *)&packet->c1 = *(u32 *)outer;
        *(u32 *)&packet->c2 = *(u32 *)inner;
        *(u32 *)&packet->c3 = *(u32 *)inner;
        packet->tag.length = 8;
        packet->c0.rgb[3] = 0x38;
        /* PSY-Q setSemiTrans(packet, D_800F337A). */
        if (D_800F337A) {
            packet->c0.rgb[3] = packet->c0.rgb[3] | 2;
        } else {
            packet->c0.rgb[3] = packet->c0.rgb[3] & ~2;
        }
        packet->tag.address = RING_OT(D_800F33B4->depth + ring->offset)->address;
        link.tag = &packet->tag;
        RING_OT(D_800F33B4->depth + ring->offset)->address = link.word;
        link.bytes = drawMode;
        link.tag->address = RING_OT(D_800F33B4->depth + ring->offset)->address;
        RING_OT(D_800F33B4->depth + ring->offset)->address = link.word;
    }
}

void func_800C5538(FieldChainRecord *record)
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
            toPlayer.x = D_8009D254->x.part.integer - link[-2].matrix.t[0];
            toPlayer.y = 0;
            toPlayer.z = D_8009D254->z.part.integer - link[-2].matrix.t[2];
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

void func_800C5A40(FieldTexturedStrip *strip)
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
    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4->clutX = strip->clut << 4;
    D_800F33B4->clutY = strip->clut >> 4;
    while (i < strip->count - 1) {
        cell = strip->cell;
        step = node->cellStep;
        D_800F33B4->v = cell & 0xF0;
        D_800F33B4->u = (strip->cell - D_800F33B4->v) << 4;
        cell += step;
        D_800F33B4->v = cell & 0xF0;
        D_800F33B4->u = (cell - D_800F33B4->v) << 4;
        quad[0] = node->edgeA;
        quad[1] = node->edgeB;
        quad[2] = node[1].edgeA;
        quad[3] = node[1].edgeB;
        {
            GteMatrix **slot;
            register const GteMatrixWords *matrix asm("$8");
            asm volatile("" : : : "memory");
            slot = &D_800BCFA4.value;
            asm volatile("" : "=r"(slot) : "0"(slot));
            matrix = (const GteMatrixWords *)*slot;
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
        }
        depth = RotTransPers4(&quad[0], &quad[1], &quad[2], &quad[3],
                              &packet->x0, &packet->x1, &packet->x2,
                              &packet->x3, &p, &flag);
        func_800C608C(node->brightness, node->rgb, &packet->r0);
        packet->u0 = D_800F33B4->u;
        packet->v0 = D_800F33B4->v;
        packet->u1 = D_800F33B4->u + D_800F345C;
        packet->v1 = D_800F33B4->v;
        packet->u2 = D_800F33B4->u;
        packet->v2 = D_800F33B4->v + D_800F345D;
        packet->u3 = D_800F33B4->u + D_800F345C;
        packet->v3 = D_800F33B4->v + D_800F345D;
        packet->tpage = D_800E27AC;
        packet->clut = GetClut(D_800F341C + D_800F33B4->clutX,
                               D_800F341E + D_800F33B4->clutY);
        {
            register u8 code asm("$8");
            u8 length = 9;
            asm("" : : "r"(length) : "$8");
            code = 0x2C;
            packet->tag.length = length;
            packet->code = code;
        }
        /* PSY-Q setSemiTrans(packet, D_800F337A). */
        if (D_800F337A) {
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
        D_8009CDD8 += sizeof(FieldStripPacket);
    }
}
