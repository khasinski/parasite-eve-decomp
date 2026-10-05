/* func_800C62DC specialised for the player position (16.16 integer halves). */
#include "pe1/field_tri_test.h"
#include "pe1/gte.h"

/* Matching debt: 7 register pins and 5 empty constraints.
 * Vector loads are C; each GTE transfer uses an individual macro. */
int func_800C689C(GteShortVector *tri)
{
    int playerX;
    register int playerZ asm("$6");
    int pointZ, vertexZ;
    GteShortVector point;
    register u32 opX asm("$12");
    register u32 opY asm("$13");
    register u32 opZ asm("$14");
    FieldTriScratch *second;
    FieldTriScratch *third;
    FieldTriScratch *result;

    playerX = D_8009D254->pos[0].integer;
    point.x = playerX;
    point.y = D_8009D254->pos[1].integer;
    playerZ = D_8009D254->pos[2].integer;
    point.z = playerZ;
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
    asm volatile("" : "=m"(D_800E2844) : "m"(D_800E2844));
    FIELD_TRI_SCRATCH->rel[0].z = point.z - tri[0].z;
    {
        register const GteVector *edge asm("$8");
        edge = &D_800E2844->edge[0];
        asm volatile("" : "=r"(edge) : "0"(edge));
        opX = edge->x;
        opY = edge->y;
        gte_ctc2_0(opX);
        opZ = edge->z;
        gte_ctc2_2(opY);
        gte_ctc2_4(opZ);
    }
    {
        register const GteVector *relative asm("$8");
        relative = &FIELD_TRI_SCRATCH->rel[0];
        gte_ldir3_precise(relative);
        gte_ldir1_precise(relative);
        gte_ldir2_precise(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point.x - tri[1].x;
    pointZ = point.z;
    vertexZ = tri[1].z;
    asm volatile("" : : : "$8");
    FIELD_TRI_SCRATCH->rel[1].z = pointZ - vertexZ;
    {
        register GteVector *cross asm("$8");
        cross = &FIELD_TRI_SCRATCH->cross[0];
        gte_swc2_25_0(cross);
        gte_swc2_26_4(cross);
        gte_swc2_27_8(cross);
    }

    second = D_800E2844;
    {
        const GteVector *edge;
        edge = &second->edge[1];
        asm volatile("" : "=r"(edge) : "0"(edge));
        opX = edge->x;
        opY = edge->y;
        gte_ctc2_0(opX);
        opZ = edge->z;
        gte_ctc2_2(opY);
        gte_ctc2_4(opZ);
    }
    {
        const GteVector *relative;
        relative = &second->rel[1];
        gte_ldir3_precise(relative);
        gte_ldir1_precise(relative);
        gte_ldir2_precise(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point.x - tri[2].x;
    second->rel[2].z = point.z - tri[2].z;
    {
        GteVector *cross;
        cross = &second->cross[1];
        gte_swc2_25_0(cross);
        gte_swc2_26_4(cross);
        gte_swc2_27_8(cross);
    }

    third = D_800E2844;
    {
        const GteVector *edge;
        edge = &third->edge[2];
        asm volatile("" : "=r"(edge) : "0"(edge));
        opX = edge->x;
        opY = edge->y;
        gte_ctc2_0(opX);
        opZ = edge->z;
        gte_ctc2_2(opY);
        gte_ctc2_4(opZ);
    }
    {
        const GteVector *relative;
        relative = &third->rel[2];
        gte_ldir3_precise(relative);
        gte_ldir1_precise(relative);
        gte_ldir2_precise(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    {
        GteVector *cross;
        cross = &third->cross[2];
        gte_swc2_25_0(cross);
        gte_swc2_26_4(cross);
        gte_swc2_27_8(cross);
    }

    result = D_800E2844;
    return ((result->cross[0].y ^ result->cross[1].y) |
            (result->cross[1].y ^ result->cross[2].y)) > 0;
}
