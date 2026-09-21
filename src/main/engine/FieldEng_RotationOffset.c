#include "pe1/gte.h"
#include "pe1/render_object.h"
void func_800CFB7C(GteShortVector *input, int arg_distance, GteShortVector *arg_out)
{
    GteShortVector *out;
    int distance;
    GteShortVector rotation;
    GteShortVector direction = D_800C2260;
    GteVector result;
    GteMatrixWords local;
    register GteMatrixWords *matrix asm("$16");
    register GteMatrixWords *saved asm("$19") = (GteMatrixWords *)D_800BCFA4.value;
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");
    /* Keep the distance argument live until the constant vector is copied. */
    {
        register int length asm("$5") = arg_distance;
        GteShortVector *output = arg_out;
        asm("" : "=r"(length), "=r"(output) : "0"(length), "1"(output));
        out = output;
        distance = length;
    }
    /* Save arguments before selecting the local matrix register. */
    asm("" : : "r"(out), "r"(distance) : "$16");
    matrix = &local;
    input->z = 0;
    rotation.x = input->x;
    rotation.y = input->y;
    rotation.z = input->z;
    RotMatrixYXZ(&rotation, (GteMatrix *)matrix);
    /* Retain matrix-relative loads across the GTE setup. */
    asm("" : "=r"(matrix) : "0"(matrix));
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
    local.tx = 0;
    local.ty = 0;
    local.tz = 0;
    x = matrix->tx;
    y = matrix->ty;
    gte_ctc2_5(x);
    z = matrix->tz;
    gte_ctc2_6(y);
    gte_ctc2_7(z);
    direction.z = distance;
    gte_lwc2_0_0(&direction);
    gte_lwc2_1_4(&direction);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);
    x = saved->r11_r12;
    y = saved->r13_r21;
    gte_ctc2_0(x);
    gte_ctc2_1(y);
    x = saved->r22_r23;
    y = saved->r31_r32;
    z = saved->r33_pad;
    gte_ctc2_2(x);
    gte_ctc2_3(y);
    gte_ctc2_4(z);
    x = saved->tx;
    y = saved->ty;
    gte_ctc2_5(x);
    z = saved->tz;
    gte_ctc2_6(y);
    gte_ctc2_7(z);
    out->x = result.x;
    out->y = result.y;
    out->z = result.z;
}
