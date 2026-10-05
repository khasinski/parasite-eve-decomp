#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_shaded_ring.h"
/* MASPSX_FLAGS: --expand-div */

/* Draw an annulus from paired inner/outer vertices.
 * Matching debt: seven register pins and eleven empty barriers. Matrix CPU
 * work is C; each GTE transfer/command and hazard nop is individually wrapped. */
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
    func_800C608C(ring->brightness, ring->outerRgb, outer);
    func_800C608C(ring->brightness, ring->innerRgb, inner);
    /* GCC 2.7.2 allocation debt: five input references keep ring in s1
     * without a hard pin that would reorder the prologue. Fewer references
     * change the allocation; this barrier emits no instructions. */
    asm("" : : "r"(ring), "r"(ring), "r"(ring), "r"(ring), "r"(ring));
    points = ring->points;
    for (i = 0; i < ring->count; i++) {
        packet = (FieldRingPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
        D_8009CDD8 += sizeof(FieldRingPacket);
        drawMode = D_800B0E58[D_8009CDDC] + D_8009CDD8;
        D_8009CDD8 += 8;
        SetDrawMode(drawMode, 0, 0, (D_800E224C & 3) << 5);
        next = (i + 1) % ring->count;
        a = i;
        c = ring->count + i;
        d = ring->count + next;
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
        packet->tag.address = RING_OT(D_800F33B4->depth + ring->depth)->address;
        link.tag = &packet->tag;
        RING_OT(D_800F33B4->depth + ring->depth)->address = link.word;
        link.bytes = drawMode;
        link.tag->address = RING_OT(D_800F33B4->depth + ring->depth)->address;
        RING_OT(D_800F33B4->depth + ring->depth)->address = link.word;
    }
}
