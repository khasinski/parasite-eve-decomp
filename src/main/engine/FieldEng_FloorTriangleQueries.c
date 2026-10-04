/* Floor-triangle containment in XZ: OP cross products of each edge with the
 * point relative to its corner; the point is inside when all Y components
 * agree in sign. */
#include "pe1/field_tri_test.h"
#include "pe1/gte.h"

int func_800C62DC(GteShortVector *point, GteShortVector *tri)
{
    FieldTriScratch *second;
    FieldTriScratch *third;
    FieldTriScratch *result;

    FIELD_TRI_SCRATCH->edge[0].y = 0;
    FIELD_TRI_SCRATCH->edge[1].y = 0;
    FIELD_TRI_SCRATCH->edge[2].y = 0;
    FIELD_TRI_SCRATCH->rel[0].y = 0;
    FIELD_TRI_SCRATCH->rel[1].y = 0;
    FIELD_TRI_SCRATCH->rel[2].y = 0;
    FIELD_TRI_SCRATCH->edge[0].x = tri[1].x - tri[0].x;
    FIELD_TRI_SCRATCH->edge[0].z = tri[1].z - tri[0].z;
    D_800E2844 = FIELD_TRI_SCRATCH;
    FIELD_TRI_SCRATCH->rel[0].x = point->x - tri[0].x;
    FIELD_TRI_SCRATCH->rel[0].z = point->z - tri[0].z;
    gte_ldopv1_psyq(&D_800E2844->edge[0]);
    gte_ldopv2(&FIELD_TRI_SCRATCH->rel[0]);
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point->x - tri[1].x;
    FIELD_TRI_SCRATCH->rel[1].z = point->z - tri[1].z;
    gte_stmac(&FIELD_TRI_SCRATCH->cross[0]);

    second = D_800E2844;
    gte_ldopv1_psyq(&second->edge[1]);
    gte_ldopv2(&second->rel[1]);
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point->x - tri[2].x;
    second->rel[2].z = point->z - tri[2].z;
    gte_stmac(&second->cross[1]);

    third = D_800E2844;
    gte_ldopv1_psyq(&third->edge[2]);
    gte_ldopv2(&third->rel[2]);
    gte_cop2_hazard_slot();
    gte_op0();
    gte_stmac(&third->cross[2]);

    result = D_800E2844;
    return ((result->cross[0].y ^ result->cross[1].y) |
            (result->cross[1].y ^ result->cross[2].y)) > 0;
}

int func_800C653C(void *arg0, char *arg1) {
    int first = func_800C62DC(arg0, (GteShortVector *)arg1);

    return first | func_800C62DC(arg0, (GteShortVector *)(arg1 + 8));
}


int func_800C6584(s16 *a, int radiusA, s16 *b, int radiusB) {
    int delta[3];
    int x = a[0] - b[0];
    int z;
    int x_sq = x * x;
    int z_sq;
    int radius;
    int radius_sq;
    delta[0] = x;
    z = a[2] - b[2];
    z_sq = z * z;
    radius = radiusA + radiusB;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}
