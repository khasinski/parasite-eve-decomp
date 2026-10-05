#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/render_object.h"

/* Matching debt: three transfer-register pins per function, plus five empty
 * pointer/memory barriers across this TU. Matrix loads are C; each GTE
 * instruction and hazard nop uses its individual macro. */

void FieldEng_TransformMatrixPoint(RoomFxTransformOwner *owner, int index,
                                  const GteShortVector *input, GteShortVector *output) {
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    GteMatrixWords *translation;
    GteVector result;
    GteMatrixWords *matrix = (GteMatrixWords *)&owner->transforms[index];
    GteMatrixWords untranslated;

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
    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    translation = &untranslated;
    asm volatile("" : "=r"(translation) : "0"(translation) : "memory");
    a = translation->tx;
    b = translation->ty;
    gte_ctc2_5(a);
    c = translation->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);

    gte_lwc2_0_0(input);
    gte_lwc2_1_4(input);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);

    /* Add translation modulo 32 bits before narrowing to halfwords. */
    output->x = result.x + owner->transforms[index].x;
    output->y = result.y + owner->transforms[index].y;
    output->z = result.z + owner->transforms[index].z;
}

void func_800CE9D4(RoomFxTransformOwner *owner, int index, GteShortVector *out)
{
    GteShortVector direction = D_800C2258;
    GteShortVector origin = D_800C2260;
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    GteMatrixWords *translation;
    GteVector result;
    GteMatrixWords local;
    GteMatrixWords *matrix;
    GteShortVector *vector = &direction;
    GteShortVector *from = &origin;

    matrix = (GteMatrixWords *)&owner->transforms[index];
    asm volatile("" : : : "memory");
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
    local.tx = local.ty = local.tz = 0;
    translation = &local;
    asm volatile("" : "=r"(translation) : "0"(translation) : "memory");
    a = translation->tx;
    b = translation->ty;
    gte_ctc2_5(a);
    c = translation->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);
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
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    GteMatrixWords *translation;
    GteVector result;
    GteMatrixWords untranslated;

    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    asm volatile("" : : : "memory");
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
    translation = &untranslated;
    asm volatile("" : "=r"(translation) : "0"(translation) : "memory");
    a = translation->tx;
    b = translation->ty;
    gte_ctc2_5(a);
    c = translation->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);

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
