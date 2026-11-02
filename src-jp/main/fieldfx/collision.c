/* CC1_FLAGS: -fno-strength-reduce */

#include "common.h"
#include "pe1/asset_find.h"
#include "pe1/battle.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_textured_strip.h"
#include "pe1/field_tri_test.h"
#include "pe1/fieldfx_state.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_libc.h"

int FieldFx_UpdateStripCollision(char *data, void *unused, int *hit) {
    GteShortVector local[5];
    FieldTexturedStrip *data_s3 = (FieldTexturedStrip *)data;
    register FieldTexturedChainNode *entry asm("$17");
    int *hit_s4;
    u32 i;
    u32 idx;
    u32 count;
    int countMinus;

    entry = data_s3->nodes;
    hit_s4 = hit;
    *hit_s4 = 0;

    idx = data_s3->collision_step;
    count = data_s3->count;
    if (idx < count) {
        entry[idx].visible = 2;
    }

    countMinus = data_s3->count;
    countMinus -= 1;
    i = 0;
    if (countMinus != 0) {
        FieldStripNodeTail *body;

        body = (FieldStripNodeTail *)&entry->brightness;
        do {
            int rnd = rand();

            body[-1].nextCellStep = rnd % 4;

            if (entry->visible == 2) {
                local[0] = body[0].edgeA;
                local[1] = body[0].edgeB;
                local[2] = body[1].edgeA;
                local[3] = body[1].edgeB;

                local[0].y = 0;
                local[1].y = 0;
                local[2].y = 0;
                local[3].y = 0;

                if (FieldFx_IsPlayerInQuadXZ(local) == 1) {
                    *hit_s4 = 1;
                }

                if (body->brightness >= 0xD) {
                    body->brightness -= 0xC;
                } else {
                    body->brightness = 0;
                    entry->visible = 0;
                }
            }
            i++;
            body++;
            entry++;
        } while (i < data_s3->count - 1);
    }

    i = (data_s3->collision_step++ == 0x64);

    return i;
}

void FieldFx_ScaleColor(int scale, u8 *src, u8 *dst) {
    scale = (short)scale;

    if (scale != 0x80) {
        int value;

        value = src[0];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[0] = value;

        value = src[1];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[1] = value;

        value = src[2];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[2] = value;
    } else {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
    }
}

int FieldFx_IsActorWithinRadius(FieldActor *obj, s16 *pos, unsigned short radius) {
    int delta[3];
    int x = Pe1Fixed_Integer(&obj->pos_x) - pos[0];
    int z;
    int x_sq = x * x;
    int z_sq;
    int radius_sq;
    delta[0] = x;
    z = Pe1Fixed_Integer(&obj->pos_z) - pos[2];
    z_sq = z * z;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}

int FieldFx_IsPointInQuadXZ(void *arg0, GteShortVector *verts);


int FieldFx_IsPointInSpriteQuad(void *arg0, GteMatrix *matrix) {
    GteShortVector verts[4];

    ApplyMatrixSV(matrix, &g_FieldFxCellQuad[0], &verts[0]);
    ApplyMatrixSV(matrix, &g_FieldFxCellQuad[1], &verts[1]);
    ApplyMatrixSV(matrix, &g_FieldFxCellQuad[2], &verts[2]);
    ApplyMatrixSV(matrix, &g_FieldFxCellQuad[3], &verts[3]);

    verts[0].x += matrix->t[0];
    verts[1].x += matrix->t[0];
    verts[2].x += matrix->t[0];
    verts[3].x += matrix->t[0];

    verts[0].y = 0;
    verts[1].y = 0;
    verts[2].y = 0;
    verts[3].y = 0;

    verts[0].z += matrix->t[2];
    verts[1].z += matrix->t[2];
    verts[2].z += matrix->t[2];
    verts[3].z += matrix->t[2];

    return FieldFx_IsPointInQuadXZ(arg0, verts) != 0;
}

/* Floor-triangle containment in XZ: OP cross products of each edge with the
 * point relative to its corner; the point is inside when all Y components
 * agree in sign. */

int FieldFx_IsPointInTriangleXZ(GteShortVector *point, GteShortVector *tri)
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
    g_FieldFxTriScratch = FIELD_TRI_SCRATCH;
    FIELD_TRI_SCRATCH->rel[0].x = point->x - tri[0].x;
    FIELD_TRI_SCRATCH->rel[0].z = point->z - tri[0].z;
    gte_ldopv1_psyq(&g_FieldFxTriScratch->edge[0]);
    gte_ldopv2(&FIELD_TRI_SCRATCH->rel[0]);
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point->x - tri[1].x;
    FIELD_TRI_SCRATCH->rel[1].z = point->z - tri[1].z;
    gte_stlvnl(&FIELD_TRI_SCRATCH->cross[0]);

    second = g_FieldFxTriScratch;
    gte_ldopv1_psyq(&second->edge[1]);
    gte_ldopv2(&second->rel[1]);
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point->x - tri[2].x;
    second->rel[2].z = point->z - tri[2].z;
    gte_stlvnl(&second->cross[1]);

    third = g_FieldFxTriScratch;
    gte_ldopv1_psyq(&third->edge[2]);
    gte_ldopv2(&third->rel[2]);
    gte_cop2_hazard_slot();
    gte_op0();
    gte_stlvnl(&third->cross[2]);

    result = g_FieldFxTriScratch;
    return ((result->cross[0].y ^ result->cross[1].y) |
            (result->cross[1].y ^ result->cross[2].y)) > 0;
}

int FieldFx_IsPointInQuadXZ(void *arg0, GteShortVector *verts) {
    int first = FieldFx_IsPointInTriangleXZ(arg0, verts);

    return first | FieldFx_IsPointInTriangleXZ(arg0, verts + 1);
}

int FieldFx_CirclesOverlapXZ(s16 *a, int radiusA, s16 *b, int radiusB) {
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

/* Floor-placement basis vectors and the asset lookups at a field
 * position; FieldFx_PlaySoundAtMatrixPosition reads the normals FieldFx_BuildPlacementBasis writes. */

void FieldFx_BuildPlacementBasis(s16 *from, s16 *to, GteMatrix *out) {
    GteVector side;
    GteVector up;
    GteVector forward;
    GteVector basis;

    basis = g_FieldPlacementUpAxis;
    forward.x = to[0] - from[0];
    forward.y = to[1] - from[1];
    forward.z = to[2] - from[2];
    OuterProduct12(&forward, &basis, &side);
    OuterProduct12(&forward, &side, &up);
    VectorNormalS(&side, (GteShortVector *)&out->t[0]);
    VectorNormalS(&up, (GteShortVector *)&out->t[1]);
    VectorNormalS(&forward, (GteShortVector *)&out->t[2]);
}

int FieldFx_PlaySoundAtMatrixPosition(FieldFxObjectPrefix *owner, int message, GteMatrix *pos) {
    int result = 0;

    if (FieldFx_GetStatus(owner) == 3 || FieldFx_GetStatus(owner) == 4) {
        result = Asset_Find08w(message, owner->actor->state->progress,
                               (s16)pos->t[0], (s16)pos->t[1], (s16)pos->t[2]);
    }

    return result;
}

int FieldFx_PlaySoundAtVectorPosition(FieldFxObjectPrefix *owner, int message, GteVector *pos) {
    int result = 0;

    if (FieldFx_GetStatus(owner) == 3 || FieldFx_GetStatus(owner) == 4) {
        result = Asset_Find08w(message, owner->actor->state->progress,
                               (s16)pos->x, (s16)pos->y, (s16)pos->z);
    }

    return result;
}

int FieldFx_FindAssetAtShortPosition(FieldFxObjectPrefix *owner, int message, GteShortVector *pos) {
    int result = 0;

    if (FieldFx_GetStatus(owner) == 3 || FieldFx_GetStatus(owner) == 4) {
        result = Asset_Find08w(message, owner->actor->state->progress,
                               pos->x, pos->y, pos->z);
    }

    return result;
}

/* FieldFx_IsPointInTriangleXZ specialised for the player position (16.16 integer halves). */
int FieldFx_IsPlayerInTriangleXZ(GteShortVector *tri)
{
    GteShortVector point;
    FieldTriScratch *second;
    FieldTriScratch *third;
    FieldTriScratch *result;

    point.x = ((FieldTriEntity *)D_8009CD10)->pos[0].integer;
    point.y = ((FieldTriEntity *)D_8009CD10)->pos[1].integer;
    point.z = ((FieldTriEntity *)D_8009CD10)->pos[2].integer;
    FIELD_TRI_SCRATCH->edge[0].y = 0;
    FIELD_TRI_SCRATCH->edge[1].y = 0;
    FIELD_TRI_SCRATCH->edge[2].y = 0;
    FIELD_TRI_SCRATCH->rel[0].y = 0;
    FIELD_TRI_SCRATCH->rel[1].y = 0;
    FIELD_TRI_SCRATCH->rel[2].y = 0;
    FIELD_TRI_SCRATCH->edge[0].x = tri[1].x - tri[0].x;
    FIELD_TRI_SCRATCH->edge[0].z = tri[1].z - tri[0].z;
    g_FieldFxTriScratch = FIELD_TRI_SCRATCH;
    FIELD_TRI_SCRATCH->rel[0].x = point.x - tri[0].x;
    FIELD_TRI_SCRATCH->rel[0].z = point.z - tri[0].z;
    gte_ldopv1_psyq(&g_FieldFxTriScratch->edge[0]);
    gte_ldopv2(&FIELD_TRI_SCRATCH->rel[0]);
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point.x - tri[1].x;
    FIELD_TRI_SCRATCH->rel[1].z = point.z - tri[1].z;
    gte_stlvnl(&FIELD_TRI_SCRATCH->cross[0]);

    second = g_FieldFxTriScratch;
    gte_ldopv1_psyq(&second->edge[1]);
    gte_ldopv2(&second->rel[1]);
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point.x - tri[2].x;
    second->rel[2].z = point.z - tri[2].z;
    gte_stlvnl(&second->cross[1]);

    third = g_FieldFxTriScratch;
    gte_ldopv1_psyq(&third->edge[2]);
    gte_ldopv2(&third->rel[2]);
    gte_cop2_hazard_slot();
    gte_op0();
    gte_stlvnl(&third->cross[2]);

    result = g_FieldFxTriScratch;
    return ((result->cross[0].y ^ result->cross[1].y) |
            (result->cross[1].y ^ result->cross[2].y)) > 0;
}

int FieldFx_IsPlayerInQuadXZ(GteShortVector *arg0) {
    GteShortVector pos;
    FieldActor *entity = (FieldActor *)D_8009CD10;
    GteShortVector *point = &pos;
    int first;

    point->x = entity->pos_x >> 16;
    point->y = entity->pos_y >> 16;
    point->z = entity->pos_z >> 16;
    first = FieldFx_IsPointInTriangleXZ(point, arg0);
    return first | FieldFx_IsPointInTriangleXZ(point, arg0 + 1);
}

int FieldFx_IsPlayerWithinRadius(s16 *pos, int extraRadius) {
    int delta[3];
    s16 local[4];
    BattleEntity *entity = D_8009CD10;
    int x = entity->posX.parts.integer;
    int radius = entity->renderObject.hit_cylinder.radius;
    int z;
    int x_sq;
    int z_sq;
    int radius_sq;
    local[0] = x;
    {
        int y;
        y = entity->posY.parts.integer;
        local[1] = y;
    }
    z = entity->posZ.parts.integer;
    local[2] = z;
    x -= pos[0];
    x_sq = x * x;
    delta[0] = x;
    z -= pos[2];
    z_sq = z * z;
    radius += extraRadius;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}


int FieldFx_RegisterOwnerContact(char *obj) {
    int enabled;

    if (FieldFx_GetStatus(obj) == 3) {
        enabled = (((FieldFxObjectPrefix *)obj)->actor->state->action_state[0] == 2);
    } else {
        enabled = 1;
    }

    if (enabled != 0) {
        if (FieldFx_GetStatus(obj) == 3) {
            ((Combatant *)D_8009CD10->core)->stateFlags |= 0x4000;
            ((FieldFxObjectPrefix *)obj)->actor->state->core_flags |= 0x80000000;
        }
    }

    return enabled;
}

int FieldFx_GetStatus(void *object) {
    FieldFxObjectPrefix *obj = object;
    FieldActor *data = obj->actor;
    int status = 0;

    if (data != 0) {
        FieldActorState *inner = data->state;

        if (inner == 0) {
            status = 1;
        } else {
            status = 3;
            /* The enemy record keeps its signed alive HP here. */
            if (inner->control10.command_value <= 0) {
                status = 2;
            }
        }
    }

    if (obj->asset_type == 7) {
        status = 4;
    }
    if (obj->asset_type == 0xE) {
        status = 4;
    }
    if (data == (FieldActor *)D_8009CD10) {
        status = 5;
    }

    return status;
}
