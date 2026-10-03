#include "fx_common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

/* Build a camera matrix looking from eye toward target with the given up
 * vector: the rows are the normalised side, up and forward axes, and the
 * translation is the rotated eye position negated. */
void func_8018F344(GteMatrix *out, GteShortVector *eye,
                   GteShortVector *target, GteVector *up)
{
    GteVector work;
    GteVector side;
    GteVector upAxis;
    GteVector forward;

    work.x = target->x - eye->x;
    work.y = target->y - eye->y;
    work.z = target->z - eye->z;
    Gte_NormalizeVec(&work, &forward);
    if (forward.z == up->z)
        forward.z++;

    gte_ldopv1_psyq(&forward);
    gte_ldopv2(up);
    gte_op12_psyq();
    gte_stmac(&work);
    Gte_NormalizeVec(&work, &side);

    gte_ldopv1_psyq(&forward);
    gte_ldopv2(&side);
    gte_op12_psyq();
    gte_stmac(&work);
    Gte_NormalizeVec(&work, &upAxis);

    out->m[0][0] = side.x;
    out->m[0][1] = side.y;
    out->m[0][2] = side.z;
    out->m[1][0] = upAxis.x;
    out->m[1][1] = upAxis.y;
    out->m[1][2] = upAxis.z;
    out->m[2][0] = forward.x;
    out->m[2][1] = forward.y;
    out->m[2][2] = forward.z;

    gte_ldrotmatrix(out);
    gte_ldv0(eye);
    gte_mvmva();
    gte_stmac(out->t);
    out->t[0] = -out->t[0];
    out->t[1] = -out->t[1];
    out->t[2] = -out->t[2];
}
