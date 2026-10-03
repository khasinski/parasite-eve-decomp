#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/render_object.h"

void FieldEng_TransformMatrixPoint(RoomFxTransformOwner *owner, int index,
                                  const GteShortVector *input, GteShortVector *output) {
    GteVector result;
    GteMatrixWords *matrix = (GteMatrixWords *)&owner->transforms[index];
    GteMatrixWords untranslated;

    gte_ldrotmatrix(matrix);
    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    gte_ldtransmatrix(&untranslated);

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
    GteVector result;
    GteMatrixWords local;
    GteMatrixWords *matrix;
    GteShortVector *vector = &direction;
    GteShortVector *from = &origin;

    matrix = (GteMatrixWords *)&owner->transforms[index];
    gte_ldrotmatrix(matrix);
    local.tx = local.ty = local.tz = 0;
    gte_ldtransmatrix(&local);
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

    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    gte_ldrotmatrix(matrix);
    gte_ldtransmatrix(&untranslated);

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
