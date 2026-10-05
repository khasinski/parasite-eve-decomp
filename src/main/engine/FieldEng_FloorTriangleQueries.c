/* Floor-triangle containment in XZ: OP cross products of each edge with the
 * point relative to its corner; the point is inside when all Y components
 * agree in sign. */
#include "pe1/field_tri_test.h"
#include "pe1/gte.h"

/* Matching debt: 6 register pins and 5 empty constraints.
 * Vector loads are C; each GTE transfer uses an individual macro. */
int func_800C62DC(GteShortVector *point, GteShortVector *tri)
{
    register u32 opX asm("$12");
    register u32 opY asm("$13");
    register u32 opZ asm("$14");
    int pointZ;
    int vertexZ;
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
    asm volatile("" : "=m"(D_800E2844) : "m"(D_800E2844));
    FIELD_TRI_SCRATCH->rel[0].z = point->z - tri[0].z;
    {
        register const GteVector *edge asm("$7");
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
        register const GteVector *relative asm("$7");
        relative = &FIELD_TRI_SCRATCH->rel[0];
        gte_lwc2_11_8(relative);
        gte_lwc2_9_0(relative);
        gte_lwc2_10_4(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point->x - tri[1].x;
    pointZ = point->z;
    vertexZ = tri[1].z;
    asm volatile("" : : : "$7");
    FIELD_TRI_SCRATCH->rel[1].z = pointZ - vertexZ;
    {
        register GteVector *cross asm("$7");
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
        gte_lwc2_11_8(relative);
        gte_lwc2_9_0(relative);
        gte_lwc2_10_4(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point->x - tri[2].x;
    second->rel[2].z = point->z - tri[2].z;
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
        gte_lwc2_11_8(relative);
        gte_lwc2_9_0(relative);
        gte_lwc2_10_4(relative);
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
