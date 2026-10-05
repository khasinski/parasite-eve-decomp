#include "pe1/gte.h"
#include "pe1/render_object.h"
/* Rotates (0, 0, distance) by the YXZ angles in `input` (roll cleared) and
 * restores the shared camera matrix afterwards. The halfword distance
 * parameter makes the callee narrow it, which lets sched1 move the argument
 * copies below the constant vector copy as retail does.
 * Matching debt: four register pins and two empty constraints preserve
 * pointer allocation and ordering. All matrix loads are C; each GTE transfer,
 * command and hazard nop is an individual macro. */
void func_800CFB7C(GteShortVector *input, s16 distance, GteShortVector *out)
{
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    GteShortVector rotation;
    GteShortVector direction = D_800C2260;
    GteVector result;
    GteMatrixWords local;
    GteMatrixWords *matrix;
    register const GteMatrixWords *saved asm("$19") = (const GteMatrixWords *)D_800BCFA4.value;

    asm("" : : "r"(out), "r"(distance) : "$16");
    matrix = &local;
    input->z = 0;
    rotation.x = input->x;
    rotation.y = input->y;
    rotation.z = input->z;
    RotMatrixYXZ(&rotation, (GteMatrix *)matrix);
    asm volatile("" : "=r"(matrix) : "0"(matrix) : "memory");
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
    local.tx = 0;
    local.ty = 0;
    local.tz = 0;
    a = matrix->tx;
    b = matrix->ty;
    gte_ctc2_5(a);
    c = matrix->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);
    direction.z = distance;
    gte_lwc2_0_0(&direction);
    gte_lwc2_1_4(&direction);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);
    a = saved->r11_r12;
    b = saved->r13_r21;
    gte_ctc2_0(a);
    gte_ctc2_1(b);
    a = saved->r22_r23;
    b = saved->r31_r32;
    c = saved->r33_pad;
    gte_ctc2_2(a);
    gte_ctc2_3(b);
    gte_ctc2_4(c);
    a = saved->tx;
    b = saved->ty;
    gte_ctc2_5(a);
    c = saved->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);
    out->x = result.x;
    out->y = result.y;
    out->z = result.z;
}
