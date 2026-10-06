/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_billboard.h"
#include "pe1/field_glow_layers.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_textured_strip.h"
#include "pe1/field_shaded_ring.h"

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
        s32 **slot;
        register const GteMatrixWords *matrix asm("$9");
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        matrix = (const GteMatrixWords *)*slot;
        a = matrix->r11_r12;
        b = matrix->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = matrix->r22_r23;
        b = matrix->r31_r32;
        c = matrix->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = matrix->tx;
        b = matrix->ty;
        gte_ctc2_5(a);
        c = matrix->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
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
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    register const GteMatrixWords *cameraRot asm("$9");
    register const GteMatrixWords *cameraTrans asm("$3");
    const GteMatrixWords *composed;
    register const GteMatrixWords *camera asm("$11");
    const GteMatrixWords *placed;
    const u16 *column;
    u16 *outColumn;
    register u16 *firstColumn asm("$10");
    s32 *outTranslation;
    const s32 *translation;
    s32 **slot;
    register GteShortVector *v0 asm("$9");
    register GteShortVector *v1 asm("$10");
    register GteShortVector *v2 asm("$11");
    register s16 *xy0 asm("$4");
    register s16 *xy1 asm("$3");
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

        a = cameraRot->r11_r12;
        b = cameraRot->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = cameraRot->r22_r23;
        b = cameraRot->r31_r32;
        c = cameraRot->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = ((const u16 *)placement)[0];
        b = ((const u16 *)placement)[3];
        c = ((const u16 *)placement)[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        firstColumn = (u16 *)&D_800F33B4->matrix;
        asm volatile("" : "=r"(firstColumn) : "0"(firstColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        firstColumn[0] = a;
        firstColumn[3] = b;
        firstColumn[6] = c;
        asm volatile("" : : : "memory");
        column = (const u16 *)placement + 1;
        asm volatile("" : "=r"(column) : "0"(column));
        a = column[0];
        b = column[3];
        c = column[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&D_800F33B4->matrix + 1;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        outColumn[0] = a;
        outColumn[3] = b;
        outColumn[6] = c;
        asm volatile("" : : : "memory");
        column = (const u16 *)placement + 2;
        asm volatile("" : "=r"(column) : "0"(column));
        a = column[0];
        b = column[3];
        c = column[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&D_800F33B4->matrix + 2;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        outColumn[0] = a;
        outColumn[3] = b;
        outColumn[6] = c;
        /* Transform placement translation with the camera matrix. */
        cameraTrans = (const GteMatrixWords *)D_800BCFA4.value;

        a = cameraTrans->tx;
        b = cameraTrans->ty;
        gte_ctc2_5(a);
        c = cameraTrans->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
        translation = placement->t;
        asm volatile("" : "=r"(translation) : "0"(translation));
        b = ((const u16 *)translation)[2];
        a = ((const u16 *)translation)[0];
        b <<= 16;
        a |= b;
        gte_mtc2_0(a);
        gte_lwc2_1_8(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        outTranslation = D_800F33B4->matrix.t;
        gte_swc2_9_0(outTranslation);
        gte_swc2_10_4(outTranslation);
        gte_swc2_11_8(outTranslation);
        composed = (const GteMatrixWords *)&D_800F33B4->matrix;
        asm volatile("" : "=r"(composed) : "0"(composed));

        a = composed->r11_r12;
        b = composed->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = composed->r22_r23;
        b = composed->r31_r32;
        c = composed->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);

        a = composed->tx;
        b = composed->ty;
        gte_ctc2_5(a);
        c = composed->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    } else {
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        camera = (const GteMatrixWords *)*slot;

        a = camera->r11_r12;
        b = camera->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = camera->r22_r23;
        b = camera->r31_r32;
        c = camera->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);

        a = camera->tx;
        b = camera->ty;
        gte_ctc2_5(a);
        c = camera->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        placed = (const GteMatrixWords *)placement;

        a = placed->r11_r12;
        b = placed->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = placed->r22_r23;
        b = placed->r31_r32;
        c = placed->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);

        a = placed->tx;
        b = placed->ty;
        gte_ctc2_5(a);
        c = placed->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
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
        gte_stsxy0_precise(xy0);
        gte_stsxy1_precise(xy1);
        gte_stsxy2_precise(xy2);
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
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    register const GteMatrixWords *cameraRot asm("$9");
    register const GteMatrixWords *cameraTrans asm("$3");
    const GteMatrixWords *composed;
    register const GteMatrixWords *camera asm("$11");
    const GteMatrixWords *placed;
    const u16 *column;
    u16 *outColumn;
    register u16 *firstColumn asm("$10");
    s32 *outTranslation;
    const s32 *translation;
    s32 **slot;
    register GteShortVector *v0 asm("$9");
    register GteShortVector *v1 asm("$10");
    register GteShortVector *v2 asm("$11");
    register s16 *xy0 asm("$4");
    register s16 *xy1 asm("$3");
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

        a = cameraRot->r11_r12;
        b = cameraRot->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = cameraRot->r22_r23;
        b = cameraRot->r31_r32;
        c = cameraRot->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = ((const u16 *)placement)[0];
        b = ((const u16 *)placement)[3];
        c = ((const u16 *)placement)[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        firstColumn = (u16 *)&D_800F33B4->matrix;
        asm volatile("" : "=r"(firstColumn) : "0"(firstColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        firstColumn[0] = a;
        firstColumn[3] = b;
        firstColumn[6] = c;
        asm volatile("" : : : "memory");
        column = (const u16 *)placement + 1;
        asm volatile("" : "=r"(column) : "0"(column));
        a = column[0];
        b = column[3];
        c = column[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&D_800F33B4->matrix + 1;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        outColumn[0] = a;
        outColumn[3] = b;
        outColumn[6] = c;
        asm volatile("" : : : "memory");
        column = (const u16 *)placement + 2;
        asm volatile("" : "=r"(column) : "0"(column));
        a = column[0];
        b = column[3];
        c = column[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&D_800F33B4->matrix + 2;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        outColumn[0] = a;
        outColumn[3] = b;
        outColumn[6] = c;
        /* Transform placement translation with the camera matrix. */
        cameraTrans = (const GteMatrixWords *)D_800BCFA4.value;

        a = cameraTrans->tx;
        b = cameraTrans->ty;
        gte_ctc2_5(a);
        c = cameraTrans->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
        translation = placement->t;
        asm volatile("" : "=r"(translation) : "0"(translation));
        b = ((const u16 *)translation)[2];
        a = ((const u16 *)translation)[0];
        b <<= 16;
        a |= b;
        gte_mtc2_0(a);
        gte_lwc2_1_8(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        outTranslation = D_800F33B4->matrix.t;
        gte_swc2_9_0(outTranslation);
        gte_swc2_10_4(outTranslation);
        gte_swc2_11_8(outTranslation);
        composed = (const GteMatrixWords *)&D_800F33B4->matrix;
        asm volatile("" : "=r"(composed) : "0"(composed));

        a = composed->r11_r12;
        b = composed->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = composed->r22_r23;
        b = composed->r31_r32;
        c = composed->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);

        a = composed->tx;
        b = composed->ty;
        gte_ctc2_5(a);
        c = composed->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    } else {
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        camera = (const GteMatrixWords *)*slot;

        a = camera->r11_r12;
        b = camera->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = camera->r22_r23;
        b = camera->r31_r32;
        c = camera->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);

        a = camera->tx;
        b = camera->ty;
        gte_ctc2_5(a);
        c = camera->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        placed = (const GteMatrixWords *)placement;

        a = placed->r11_r12;
        b = placed->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = placed->r22_r23;
        b = placed->r31_r32;
        c = placed->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);

        a = placed->tx;
        b = placed->ty;
        gte_ctc2_5(a);
        c = placed->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
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
        gte_stsxy0_precise(xy0);
        gte_stsxy1_precise(xy1);
        gte_stsxy2_precise(xy2);
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
    register u32 wordA asm("$12");
    register u32 wordB asm("$13");
    register u32 wordC asm("$14");
    register const GteMatrixWords *cameraRot asm("$10");
    register const GteMatrixWords *cameraTrans asm("$3");
    const GteMatrixWords *composed;
    register const GteMatrixWords *camera asm("$10");
    const GteMatrixWords *placed;
    const u16 *column;
    u16 *outColumn;
    register u16 *firstColumn asm("$10");
    s32 *outTranslation;
    const s32 *translation;
    s32 **slot;
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
        cameraRot = (const GteMatrixWords *)D_800BCFA4.value;

        wordA = cameraRot->r11_r12;
        wordB = cameraRot->r13_r21;
        gte_ctc2_0(wordA);
        gte_ctc2_1(wordB);
        wordA = cameraRot->r22_r23;
        wordB = cameraRot->r31_r32;
        wordC = cameraRot->r33_pad;
        gte_ctc2_2(wordA);
        gte_ctc2_3(wordB);
        gte_ctc2_4(wordC);
        wordA = ((const u16 *)placement)[0];
        wordB = ((const u16 *)placement)[3];
        wordC = ((const u16 *)placement)[6];
        gte_mtc2_9(wordA);
        gte_mtc2_10(wordB);
        gte_mtc2_11(wordC);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        firstColumn = (u16 *)&D_800F33B4->matrix;
        asm volatile("" : "=r"(firstColumn) : "0"(firstColumn));
        gte_mfc2_9(wordA);
        gte_mfc2_10(wordB);
        gte_mfc2_11(wordC);
        firstColumn[0] = wordA;
        firstColumn[3] = wordB;
        firstColumn[6] = wordC;
        asm volatile("" : : : "memory");
        column = (const u16 *)placement + 1;
        asm volatile("" : "=r"(column) : "0"(column));
        wordA = column[0];
        wordB = column[3];
        wordC = column[6];
        gte_mtc2_9(wordA);
        gte_mtc2_10(wordB);
        gte_mtc2_11(wordC);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&D_800F33B4->matrix + 1;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(wordA);
        gte_mfc2_10(wordB);
        gte_mfc2_11(wordC);
        outColumn[0] = wordA;
        outColumn[3] = wordB;
        outColumn[6] = wordC;
        asm volatile("" : : : "memory");
        column = (const u16 *)placement + 2;
        asm volatile("" : "=r"(column) : "0"(column));
        wordA = column[0];
        wordB = column[3];
        wordC = column[6];
        gte_mtc2_9(wordA);
        gte_mtc2_10(wordB);
        gte_mtc2_11(wordC);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&D_800F33B4->matrix + 2;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(wordA);
        gte_mfc2_10(wordB);
        gte_mfc2_11(wordC);
        outColumn[0] = wordA;
        outColumn[3] = wordB;
        outColumn[6] = wordC;
        /* Transform placement translation with the camera matrix. */
        cameraTrans = (const GteMatrixWords *)D_800BCFA4.value;

        wordA = cameraTrans->tx;
        wordB = cameraTrans->ty;
        gte_ctc2_5(wordA);
        wordC = cameraTrans->tz;
        gte_ctc2_6(wordB);
        gte_ctc2_7(wordC);
        translation = placement->t;
        asm volatile("" : "=r"(translation) : "0"(translation));
        wordB = ((const u16 *)translation)[2];
        wordA = ((const u16 *)translation)[0];
        wordB <<= 16;
        wordA |= wordB;
        gte_mtc2_0(wordA);
        gte_lwc2_1_8(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        outTranslation = D_800F33B4->matrix.t;
        gte_swc2_9_0(outTranslation);
        gte_swc2_10_4(outTranslation);
        gte_swc2_11_8(outTranslation);
        composed = (const GteMatrixWords *)&D_800F33B4->matrix;
        asm volatile("" : "=r"(composed) : "0"(composed));

        wordA = composed->r11_r12;
        wordB = composed->r13_r21;
        gte_ctc2_0(wordA);
        gte_ctc2_1(wordB);
        wordA = composed->r22_r23;
        wordB = composed->r31_r32;
        wordC = composed->r33_pad;
        gte_ctc2_2(wordA);
        gte_ctc2_3(wordB);
        gte_ctc2_4(wordC);

        wordA = composed->tx;
        wordB = composed->ty;
        gte_ctc2_5(wordA);
        wordC = composed->tz;
        gte_ctc2_6(wordB);
        gte_ctc2_7(wordC);
    } else {
        slot = &D_800BCFA4.value;
        asm volatile("" : "=r"(slot) : "0"(slot));
        camera = (const GteMatrixWords *)*slot;

        wordA = camera->r11_r12;
        wordB = camera->r13_r21;
        gte_ctc2_0(wordA);
        gte_ctc2_1(wordB);
        wordA = camera->r22_r23;
        wordB = camera->r31_r32;
        wordC = camera->r33_pad;
        gte_ctc2_2(wordA);
        gte_ctc2_3(wordB);
        gte_ctc2_4(wordC);

        wordA = camera->tx;
        wordB = camera->ty;
        gte_ctc2_5(wordA);
        wordC = camera->tz;
        gte_ctc2_6(wordB);
        gte_ctc2_7(wordC);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        placed = (const GteMatrixWords *)placement;

        wordA = placed->r11_r12;
        wordB = placed->r13_r21;
        gte_ctc2_0(wordA);
        gte_ctc2_1(wordB);
        wordA = placed->r22_r23;
        wordB = placed->r31_r32;
        wordC = placed->r33_pad;
        gte_ctc2_2(wordA);
        gte_ctc2_3(wordB);
        gte_ctc2_4(wordC);

        wordA = placed->tx;
        wordB = placed->ty;
        gte_ctc2_5(wordA);
        wordC = placed->tz;
        gte_ctc2_6(wordB);
        gte_ctc2_7(wordC);
    }
    func_800C608C(ring->intensity, ring->color1, outer);
    func_800C608C(ring->intensity, ring->color0, inner);
    /* GCC 2.7.2 allocation debt: five input references keep ring in s1
     * without a hard pin that would reorder the prologue. Fewer references
     * change the allocation; this barrier emits no instructions. */
    asm("" : : "r"(ring), "r"(ring), "r"(ring), "r"(ring), "r"(ring));
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
