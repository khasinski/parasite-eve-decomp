/* func_800C62DC specialised for the player position (16.16 integer halves). */
#include "pe1/field_tri_test.h"
#include "pe1/gte.h"

int func_800C689C(GteShortVector *tri)
{
    GteShortVector point;
    FieldTriScratch *second;
    FieldTriScratch *third;
    FieldTriScratch *result;

    point.x = D_8009D254->pos[0].integer;
    point.y = D_8009D254->pos[1].integer;
    point.z = D_8009D254->pos[2].integer;
    FIELD_TRI_SCRATCH->edge[0].y = 0;
    FIELD_TRI_SCRATCH->edge[1].y = 0;
    FIELD_TRI_SCRATCH->edge[2].y = 0;
    FIELD_TRI_SCRATCH->rel[0].y = 0;
    FIELD_TRI_SCRATCH->rel[1].y = 0;
    FIELD_TRI_SCRATCH->rel[2].y = 0;
    FIELD_TRI_SCRATCH->edge[0].x = tri[1].x - tri[0].x;
    FIELD_TRI_SCRATCH->edge[0].z = tri[1].z - tri[0].z;
    D_800E2844 = FIELD_TRI_SCRATCH;
    FIELD_TRI_SCRATCH->rel[0].x = point.x - tri[0].x;
    FIELD_TRI_SCRATCH->rel[0].z = point.z - tri[0].z;
    gte_ldopv1_psyq(&D_800E2844->edge[0]);
    gte_ldopv2(&FIELD_TRI_SCRATCH->rel[0]);
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point.x - tri[1].x;
    FIELD_TRI_SCRATCH->rel[1].z = point.z - tri[1].z;
    gte_stmac(&FIELD_TRI_SCRATCH->cross[0]);

    second = D_800E2844;
    gte_ldopv1_psyq(&second->edge[1]);
    gte_ldopv2(&second->rel[1]);
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point.x - tri[2].x;
    second->rel[2].z = point.z - tri[2].z;
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
