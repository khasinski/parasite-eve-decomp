#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_glow_layers.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_textured_strip.h"

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
