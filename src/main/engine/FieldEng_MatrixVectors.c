#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/render_object.h"

void FieldEng_TransformMatrixPoint(RoomFxTransformOwner *owner, int index,
                                  const GteShortVector *input, GteShortVector *output) {
    GteVector result;
    GteMatrixWords *matrix = (GteMatrixWords *)&owner->transforms[index];
    GteMatrixWords untranslated;
    GteMatrixWords *translation;
    /* Retail GTE transfer staging; tracked in crutch debt. */
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");

    x = matrix->r11_r12;
    y = matrix->r13_r21;
    gte_ctc2_0(x);
    gte_ctc2_1(y);
    x = matrix->r22_r23;
    y = matrix->r31_r32;
    z = matrix->r33_pad;
    gte_ctc2_2(x);
    gte_ctc2_3(y);
    gte_ctc2_4(z);

    translation = &untranslated;
    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    /* Retain the matrix-base addressing used by the transfer sequence. */
    asm volatile("" : "=r"(translation) : "0"(translation));
    x = translation->tx;
    y = translation->ty;
    gte_ctc2_5(x);
    z = translation->tz;
    gte_ctc2_6(y);
    gte_ctc2_7(z);

    gte_lwc2_0_0(input);
    gte_lwc2_1_4(input);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);

    /* Add translation modulo 32 bits before narrowing to halfwords. */
    output->x = (u32)result.x + owner->transforms[index].x;
    output->y = (u32)result.y + owner->transforms[index].y;
    output->z = (u32)result.z + owner->transforms[index].z;
}

void func_800CE9D4(RoomFxTransformOwner *owner, int index, GteShortVector *out)
{
    GteShortVector direction = D_800C2258;
    GteShortVector origin = D_800C2260;
    GteVector result;
    GteMatrixWords local;
    GteMatrixWords *matrix;
    GteShortVector *vector = &direction;
    GteShortVector *from = &origin;
    /* Retail GTE transfer registers; all three pins are tracked in debt. */
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");

    /* Retain both local-vector addresses before selecting the matrix. */
    asm volatile("" : : "r"(vector), "r"(from) : "memory");
    matrix = (GteMatrixWords *)&owner->transforms[index];
    x = matrix->r11_r12;
    y = matrix->r13_r21;
    gte_ctc2_0(x);
    gte_ctc2_1(y);
    x = matrix->r22_r23;
    y = matrix->r31_r32;
    z = matrix->r33_pad;
    gte_ctc2_2(x);
    gte_ctc2_3(y);
    gte_ctc2_4(z);
    matrix = &local;
    local.tx = local.ty = local.tz = 0;
    /* Preserve the original loads from the zeroed translation fields. */
    asm("" : "=r"(matrix) : "0"(matrix));
    x = matrix->tx;
    y = matrix->ty;
    gte_ctc2_5(x);
    z = matrix->tz;
    gte_ctc2_6(y);
    gte_ctc2_7(z);
    gte_lwc2_0_0(vector);
    gte_lwc2_1_4(vector);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);
    direction.x = result.x;
    direction.y = result.y;
    direction.z = result.z;
    FieldEng_CalculateLookAngles(from, vector, out);
}

void FieldEng_RotateVector(const GteMatrixWords *matrix,
                           const GteShortVector *input, GteShortVector *output) {
    GteVector result;
    GteMatrixWords untranslated;
    GteMatrixWords *translation;
    /* Retail GTE transfer staging; tracked in crutch debt. */
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");

    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    /* Preserve the zero-translation stores before the matrix loads. */
    asm volatile("" : : : "memory");
    x = matrix->r11_r12;
    y = matrix->r13_r21;
    gte_ctc2_0(x);
    gte_ctc2_1(y);
    x = matrix->r22_r23;
    y = matrix->r31_r32;
    z = matrix->r33_pad;
    gte_ctc2_2(x);
    gte_ctc2_3(y);
    gte_ctc2_4(z);

    translation = &untranslated;
    /* Retain the matrix-base addressing used by the transfer sequence. */
    asm volatile("" : "=r"(translation) : "0"(translation));
    x = translation->tx;
    y = translation->ty;
    gte_ctc2_5(x);
    z = translation->tz;
    gte_ctc2_6(y);
    gte_ctc2_7(z);

    gte_lwc2_0_0(input);
    gte_lwc2_1_4(input);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);

    output->x = result.x;
    output->y = result.y;
    output->z = result.z;
}
