#include "pe1/gte.h"
#include "pe1/render_object.h"
/* Rotates (0, 0, distance) by the YXZ angles in `input` (roll cleared) and
 * restores the shared camera matrix afterwards. The halfword distance
 * parameter makes the callee narrow it, which lets sched1 move the argument
 * copies below the constant vector copy as retail does. */
void func_800CFB7C(GteShortVector *input, s16 distance, GteShortVector *out)
{
    GteShortVector rotation;
    GteShortVector direction = D_800C2260;
    GteVector result;
    GteMatrixWords local;
    s32 *saved = D_800BCFA4.value;

    input->z = 0;
    rotation.x = input->x;
    rotation.y = input->y;
    rotation.z = input->z;
    RotMatrixYXZ(&rotation, (GteMatrix *)&local);
    gte_ldrotmatrix(&local);
    local.tx = 0;
    local.ty = 0;
    local.tz = 0;
    gte_ldtransmatrix(&local);
    direction.z = distance;
    gte_lwc2_0_0(&direction);
    gte_lwc2_1_4(&direction);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);
    gte_ldrotmatrix(saved);
    gte_ldtransmatrix(saved);
    out->x = result.x;
    out->y = result.y;
    out->z = result.z;
}
