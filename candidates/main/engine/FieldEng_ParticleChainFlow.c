#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_particle_chain.h"
#include "pe1/field_textured_strip.h"

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
    register GteMatrix *workMatrix asm("$20");
    register GteMatrix *stepMatrix asm("$21");

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
        stepMatrix = &rotation;
        workMatrix = &matrix;
        asm("" : "=r"(workMatrix), "=r"(stepMatrix) : "0"(workMatrix), "1"(stepMatrix));
        ApplyMatrixSV(workMatrix, &tailIn, &tail);
        ApplyMatrixSV(workMatrix, &headIn, &head);
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
            run = Gte_ISqrt(previous.x * previous.x + previous.z * previous.z);
            Gte_Atan2(run, Gte_ISqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z));
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
        RotMatrix(&angles, stepMatrix);
        rotation.t[0] = 0;
        rotation.t[1] = 0;
        rotation.t[2] = -record->depth;
        {
            register u32 a asm("$12");
            register u32 b asm("$13");
            register u32 c asm("$14");
            const GteMatrixWords *cameraRot;
            const GteMatrixWords *cameraTrans;
            const u16 *column;
            u16 *outColumn;
            u16 *firstColumn;
            s32 *outTranslation;
            const s32 *translation;
            asm volatile("" : : : "memory");
            cameraRot = (const GteMatrixWords *)workMatrix;

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
            a = ((const u16 *)stepMatrix)[0];
            b = ((const u16 *)stepMatrix)[3];
            c = ((const u16 *)stepMatrix)[6];
            gte_mtc2_9(a);
            gte_mtc2_10(b);
            gte_mtc2_11(c);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_mvmva_rotation_ir_sf12();
            firstColumn = (u16 *)workMatrix;

            gte_mfc2_9(a);
            gte_mfc2_10(b);
            gte_mfc2_11(c);
            firstColumn[0] = a;
            firstColumn[3] = b;
            firstColumn[6] = c;
            asm volatile("" : : : "memory");
            column = (const u16 *)&rotation + 1;
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
            outColumn = (u16 *)&matrix + 1;
            asm volatile("" : "=r"(outColumn) : "0"(outColumn));
            gte_mfc2_9(a);
            gte_mfc2_10(b);
            gte_mfc2_11(c);
            outColumn[0] = a;
            outColumn[3] = b;
            outColumn[6] = c;
            asm volatile("" : : : "memory");
            column = (const u16 *)&rotation + 2;
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
            outColumn = (u16 *)&matrix + 2;
            asm volatile("" : "=r"(outColumn) : "0"(outColumn));
            gte_mfc2_9(a);
            gte_mfc2_10(b);
            gte_mfc2_11(c);
            outColumn[0] = a;
            outColumn[3] = b;
            outColumn[6] = c;
            /* Transform placement translation with the camera matrix. */
            asm volatile("" : : : "memory");
            cameraTrans = (const GteMatrixWords *)workMatrix;

            a = cameraTrans->tx;
            b = cameraTrans->ty;
            gte_ctc2_5(a);
            c = cameraTrans->tz;
            gte_ctc2_6(b);
            gte_ctc2_7(c);
            translation = rotation.t;
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
            outTranslation = matrix.t;
            gte_swc2_9_0(outTranslation);
            gte_swc2_10_4(outTranslation);
            gte_swc2_11_8(outTranslation);
        }
        link->matrix = matrix;
    }
}
