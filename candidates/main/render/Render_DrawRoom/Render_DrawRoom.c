#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_camera.h"
#include "pe1/render_prim.h"
#include "pe1/field_tile.h"
#include "pe1/field_anim.h"
#include "pe1/render_shadow.h"

/* Builds the ground-aligned shadow transform for `actor` and links its
 * shadow quad, projected from a square of the model's shadow radius. */
int Render_DrawRoom(RenderShadowActor *actor)
{
    GteShortVector origin;
    GteShortVector corner;
    GteVector axis;
    GteVector normal;
    GteVector up;
    GteMatrixStorage local;
    GteMatrix world;
    s32 sxy;
    s32 p;
    s32 flag;
    RenderShadowQuad *quad;
    GteMatrixStorage *source;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int radius;
    int shade;
    int depth;
    int maxDepth;

    if (actor->flags & 0x400)
        return 0;
    if (!actor->visible)
        return 0;

    radius = actor->header->shadow_radius;
    origin.x = actor->x.integer;
    origin.y = actor->y.integer;
    origin.z = actor->z.integer;
    source = &actor->matrices[actor->shadow_matrix_index];
    local.words[1] = source->words[1];
    local.words[2] = source->words[2];
    local.words[3] = source->words[3];
    local.words[4] = source->words[4];
    local.words[5] = source->words[5];
    local.words[6] = source->words[6];
    local.matrix.t[0] = 0;
    local.words[0] = source->words[0];
    corner.z = 0x1000;
    local.words[7] = source->words[7];
    corner.x = 0;
    local.matrix.t[2] = 0;
    corner.y = 0;
    local.matrix.t[1] = 0;
    gte_ldrotmatrix(local.words);
    gte_ldtransmatrix(local.words);
    gte_ldv0(&corner);
    gte_rt();
    gte_stmac(&axis);
    axis.y = 0;
    if (axis.x == 0 && axis.z == 0) {
        normal.x = 0;
        normal.y = 0;
        normal.z = 0x1000;
    } else {
        Gte_NormalizeVec(&axis, &normal);
    }
    local.matrix.m[1][1] = 0x1000;
    up.y = 0x1000;
    local.matrix.m[0][1] = 0;
    local.matrix.m[2][1] = 0;
    up.x = 0;
    up.z = 0;
    local.matrix.m[0][2] = normal.x;
    local.matrix.m[1][2] = normal.y;
    local.matrix.m[2][2] = normal.z;
    gte_ldopv1_psyq(&normal);
    gte_ldopv2(&up);
    gte_op12_psyq();
    gte_stmac(&axis);
    local.matrix.m[0][0] = axis.x;
    local.matrix.m[1][0] = axis.y;
    local.matrix.m[2][0] = axis.z;
    if (actor->flags & 0x4000000)
        local.matrix.t[1] = D_800942EC;
    else
        local.matrix.t[1] = origin.y;
    local.matrix.t[0] = actor->matrices[actor->shadow_matrix_index].matrix.t[0];
    local.matrix.t[2] = actor->matrices[actor->shadow_matrix_index].matrix.t[2];
    gte_CompMatrix(D_800B89F8, &local.matrix, &world);
    SetRotMatrix(&world);
    SetTransMatrix(&world);
    SetGeomScreen(D_800B89F8[8]);

    quad = &actor->shadow_quads[D_8009CDDC];
    quad->tag.length = 9;
    quad->code = 0x2C;
    if (actor->render_flags & 6)
        shade = (actor->fade_red + actor->fade_green + actor->fade_blue) / 3;
    else
        shade = 0x80;
    shade = shade * actor->shadow_intensity / 128;
    {
        int half = shade / 2;

        quad->r0 = half;
        quad->g0 = half;
        quad->b0 = half;
    }
    quad->u0 = 0;
    quad->v0 = 0x40;
    quad->u1 = 0x3F;
    quad->v1 = 0x40;
    quad->u2 = 0;
    quad->v2 = 0x7F;
    quad->u3 = 0x3F;
    quad->v3 = 0x7F;
    quad->clut = 0x7210;
    quad->tpage = 0xCB;
    quad->code |= 2;

    maxDepth = -1;
    corner.y = 0;
    corner.x = -radius;
    corner.z = radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy0 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;
    corner.x = radius;
    corner.z = radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy1 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;
    corner.x = -radius;
    corner.z = -radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy2 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;
    corner.x = radius;
    corner.z = -radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy3 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;

    if (maxDepth >= 0 && maxDepth < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], maxDepth);
        quad->tag.address = ot.tag->address;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], maxDepth);
        link.tag = &quad->tag;
        ot.tag->address = link.word;
    }
    return 0;
}
