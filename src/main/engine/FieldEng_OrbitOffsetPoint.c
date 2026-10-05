#include "pe1/field_orbit_point.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

/* Offset a point by a radius along the rotated Z axis and a roll-rotated X
 * arm, then restore the field camera matrix.
 * Matching debt: six register pins and three empty pointer barriers.
 * Matrix loads are C; GTE instructions and hazard nops use individual macros. */
void func_800CF844(GteShortVector *origin, GteShortVector *out, int radius,
                   GteShortVector *angles, int arm, int roll)
{
    GteShortVector forward;
    GteShortVector side;
    GteVector point;
    GteMatrix rotation;
    GteMatrix rollMatrix;
    GteMatrixWords *matrix;
    register GteMatrixWords *rolled asm("$16");
    register GteMatrixWords *camera asm("$8");
    s32 **slot;
    register GteShortVector *rotationAngles asm("$4");
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");

    rollMatrix = D_800C2270;
    forward.y = 0;
    forward.x = 0;
    forward.z = radius;
    rotationAngles = angles;
    matrix = (GteMatrixWords *)&rotation;
    RotMatrixYXZ(rotationAngles, (GteMatrix *)matrix);
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
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = 0;
    a = matrix->tx;
    b = matrix->ty;
    gte_ctc2_5(a);
    c = matrix->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);
    gte_lwc2_0_0(&forward);
    gte_lwc2_1_4(&forward);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    side.z = 0;
    side.y = 0;
    side.x = arm;
    gte_swc2_25_0(rotation.t);
    gte_swc2_26_4(rotation.t);
    gte_swc2_27_8(rotation.t);
    rolled = (GteMatrixWords *)&rollMatrix;
    RotMatrixZ(roll, (GteMatrix *)rolled);
    MulRotMatrix((GteMatrix *)rolled);
    asm volatile("" : "=r"(rolled) : "0"(rolled) : "memory");
    a = rolled->r11_r12;
    b = rolled->r13_r21;
    gte_ctc2_0(a);
    gte_ctc2_1(b);
    a = rolled->r22_r23;
    b = rolled->r31_r32;
    c = rolled->r33_pad;
    gte_ctc2_2(a);
    gte_ctc2_3(b);
    gte_ctc2_4(c);
    a = matrix->tx;
    b = matrix->ty;
    gte_ctc2_5(a);
    c = matrix->tz;
    gte_ctc2_6(b);
    gte_ctc2_7(c);
    gte_lwc2_0_0(&side);
    gte_lwc2_1_4(&side);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_25_0(&point);
    gte_swc2_26_4(&point);
    gte_swc2_27_8(&point);
    slot = &D_800BCFA4.value;
    asm volatile("" : "=r"(slot) : "0"(slot));
    camera = (GteMatrixWords *)*slot;
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
    point.x += origin->x;
    point.y += origin->y;
    point.z += origin->z;
    out->x = point.x;
    out->y = point.y;
    out->z = point.z;
}
