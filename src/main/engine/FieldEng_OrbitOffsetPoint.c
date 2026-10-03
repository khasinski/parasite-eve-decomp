#include "pe1/field_orbit_point.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

/* Offset a point by a radius along the rotated Z axis and a roll-rotated X
 * arm, then restore the field camera matrix. */
void func_800CF844(GteShortVector *origin, GteShortVector *out, int radius,
                   GteShortVector *angles, int arm, int roll)
{
    GteShortVector forward;
    GteShortVector side;
    GteVector point;
    GteMatrix rotation;
    GteMatrix rollMatrix;

    rollMatrix = D_800C2270;
    forward.y = 0;
    forward.x = 0;
    forward.z = radius;
    RotMatrixYXZ(angles, &rotation);
    gte_ldrotmatrix(&rotation);
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = 0;
    gte_ldtransmatrix(&rotation);
    gte_ldv0(&forward);
    gte_rtv0tr_mac();
    side.z = 0;
    side.y = 0;
    side.x = arm;
    gte_stmac(rotation.t);
    RotMatrixZ(roll, &rollMatrix);
    MulRotMatrix(&rollMatrix);
    gte_ldrotmatrix(&rollMatrix);
    gte_ldtransmatrix(&rotation);
    gte_ldv0(&side);
    gte_rtv0tr_mac();
    gte_stmac(&point);
    gte_ldrotmatrix(D_800BCFA4.value);
    gte_ldtransmatrix(D_800BCFA4.value);
    point.x += origin->x;
    point.y += origin->y;
    point.z += origin->z;
    out->x = point.x;
    out->y = point.y;
    out->z = point.z;
}
