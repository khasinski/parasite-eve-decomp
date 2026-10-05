/* WIP, not promoted: func_800C3324 has linked score 90, size 2016 bytes.
 * CPU work formerly hidden by gte_CompMatrix is expanded into C; GTE
 * instructions and hazard nops have individual macros.
 * Remaining differences: at 800C35BC/800C361C/800C367C, the final X product
 * uses a3 instead of v1 (and each following addu differs). The translation
 * store at 800C36A0 is one instruction early. Eight linked diff rows total.
 * Current candidate debt: fourteen pins and fourteen empty barriers.
 * Native stock GCC 2.7.2 and MASPSX on darwine; no toolchain modifications.
 * 528 targeted source/allocation variants were tested before retaining the
 * original readable arithmetic. A 90-second permuter run completed 24,003
 * iterations (75 rejected compilations) without improving score 90; stopped.
 * Research: scratch/oriented_sprite and /home/hasik/fx-search-archives/
 * oriented_sprite on darwine. Production retains its matching legacy version.
 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_oriented_sprite.h"

void func_800C3324(FieldOrientedSprite *sprite)
{
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    register const GteMatrixWords *cameraRot asm("$3");
    const GteMatrixWords *cameraTrans;
    const GteMatrixWords *composed;
    register const u16 *column asm("$3");
    register const u16 *firstSource asm("$2");
    register u16 *outColumn asm("$2");
    u16 *firstColumn;
    const s32 *translation;
    register s32 *outTranslation asm("$3");
    register FieldEngineScratch *scratch asm("$4");
    FieldEngineScratch *nextScratch;
    FieldEngineScratch *translationScratch;
    register GteShortVector *v0 asm("$9");
    register GteShortVector *v1 asm("$10");
    register GteShortVector *v2 asm("$11");
    register s16 *xy0 asm("$4");
    register s16 *xy1 asm("$3");
    s16 *xy2;
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
    Gte_ScaleMatrix(&D_800F33B4->local, &sprite->scale);
    D_800F33B4->local.t[0] = 0;
    D_800F33B4->local.t[1] = 0;
    D_800F33B4->local.t[2] = 0;
    D_800F33B4->matrix = *D_800BCFA4.matrix;
    D_800F33B4->matrix.t[0] += (D_800F33B4->matrix.m[0][2] * sprite->position.z +
                                D_800F33B4->matrix.m[0][1] * sprite->position.y +
                                D_800F33B4->matrix.m[0][0] * sprite->position.x) / 4096;
    D_800F33B4->matrix.t[1] += (D_800F33B4->matrix.m[1][2] * sprite->position.z +
                                D_800F33B4->matrix.m[1][1] * sprite->position.y +
                                D_800F33B4->matrix.m[1][0] * sprite->position.x) / 4096;
    D_800F33B4->matrix.t[2] += (D_800F33B4->matrix.m[2][2] * sprite->position.z +
                                D_800F33B4->matrix.m[2][1] * sprite->position.y +
                                D_800F33B4->matrix.m[2][0] * sprite->position.x) / 4096;
    asm volatile("" : : : "$3");
    scratch = D_800F33B4;
    cameraRot = (const GteMatrixWords *)&scratch->matrix;
    asm volatile("" : "=r"(cameraRot) : "0"(cameraRot));

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
    firstSource = (const u16 *)&scratch->local;
    asm volatile("" : "=r"(firstSource) : "0"(firstSource));
    a = firstSource[0];
    b = firstSource[3];
    c = firstSource[6];
    gte_mtc2_9(a);
    gte_mtc2_10(b);
    gte_mtc2_11(c);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_ir_sf12();
    firstColumn = (u16 *)cameraRot;
    asm volatile("" : "=r"(firstColumn) : "0"(firstColumn));
    gte_mfc2_9(a);
    gte_mfc2_10(b);
    gte_mfc2_11(c);
    firstColumn[0] = a;
    firstColumn[3] = b;
    firstColumn[6] = c;
    asm volatile("" : : : "memory");
    nextScratch = D_800F33B4;
    column = (const u16 *)&nextScratch->local + 1;
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
    outColumn = (u16 *)&nextScratch->matrix + 1;
    asm volatile("" : "=r"(outColumn) : "0"(outColumn));
    gte_mfc2_9(a);
    gte_mfc2_10(b);
    gte_mfc2_11(c);
    outColumn[0] = a;
    outColumn[3] = b;
    outColumn[6] = c;
    asm volatile("" : : : "memory");
    nextScratch = D_800F33B4;
    column = (const u16 *)&nextScratch->local + 2;
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
    outColumn = (u16 *)&nextScratch->matrix + 2;
    asm volatile("" : "=r"(outColumn) : "0"(outColumn));
    gte_mfc2_9(a);
    gte_mfc2_10(b);
    gte_mfc2_11(c);
    outColumn[0] = a;
    outColumn[3] = b;
    outColumn[6] = c;
    asm volatile("" : : : "memory");
    /* Transform placement translation with the camera matrix. */
    translationScratch = D_800F33B4;
    cameraTrans = (const GteMatrixWords *)&translationScratch->matrix;
    asm volatile("" : "=r"(cameraTrans) : "0"(cameraTrans));

    a = cameraTrans->tx;
    b = cameraTrans->ty;
    gte_ctc2_5(a);
    c = cameraTrans->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);
    translation = translationScratch->local.t;
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
    outTranslation = translationScratch->matrix.t;
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
    packet->tag.address = STRIP_OT(D_800F33B4->depth + sprite->depth)->address;
    link.tag = &packet->tag;
    STRIP_OT(D_800F33B4->depth + sprite->depth)->address = link.word;
    D_8009CDD8 += sizeof(FieldStripPacket);
}
