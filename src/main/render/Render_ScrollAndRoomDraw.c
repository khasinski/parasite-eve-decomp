/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/game_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_camera.h"
#include "pe1/render_prim.h"
#include "pe1/field_tile.h"
#include "pe1/field_anim.h"
#include "pe1/render_shadow.h"

/* Historical name: applies texture scrolling and camera-relative parallax. */
int Scene_IsBattleMode(void)
{
    if (!(g_GameStateFlags & 0x104)) {
        int i;
        GeomState *state = D_800B1624;
        GeomStateAddress table;
        GeomEntryView *entries;
        int count;

        table.state = D_800B1624;
        table.word += state->entry_offset;
        entries = table.views;
        count = state->entry_count06;

        for (i = 0; i < count; i++) {
            GeomScrollEntry *entry = &entries[i].scroll;
            int position;

            if (entry->flags & 4) {
                int x, y, fracX;

                position = (entry->x * 256) | entry->fractionX.byte;
                position += entry->speedX;
                x = (position >> 8) % entry->modulusX;
                fracX = position & 255;
                position = (entry->y * 256) | entry->fractionY.byte;
                position += entry->speedY;
                y = (position >> 8) % entry->modulusY;
                entry->fractionX.word = fracX;
                entry->fractionY.word = position & 255;
                entry->x = x;
                entry->y = y;
            }
            if (entry->flags & 8) {
                /* The final fixed-point sum wraps at the target word width. */
                position = (unsigned int)(entry->baseX * 256) +
                    (D_800BCF8C.x - D_800BCF8C.originX) * entry->speedX;
                entry->x = position >> 8;
                entry->fractionX.word = position & 255;
                position = (unsigned int)(entry->baseY * 256) +
                    (D_800BCF8C.y - D_800BCF8C.originY) * entry->speedY;
                entry->y = position >> 8;
                entry->fractionY.word = position & 255;
            }
        }
        if (D_800BCF88.state.flags & 0x80) {
            unsigned short x = D_800BCF8C.x, y = D_800BCF8E;
            D_800BCF88.state.flags &= ~0x80;
            D_800BCF90 = x;
            D_800BCF92 = y;
        }
    }
    return 0;
}

/* Builds the ground-aligned shadow transform for `actor` and links its
 * shadow quad, projected from a square of the model's shadow radius.
 * Matching debt: 3 register pins and 18 empty constraints. GTE transfers
 * and commands are wrapped individually; matrix arithmetic and loads are C. */
int Render_DrawRoom(RenderShadowActor *actor)
{
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
    GteShortVector origin;
    GteShortVector corner;
    GteVector axis;
    GteVector normal;
    s32 normalX, normalY, normalZ;
    GteVector up;
    GteMatrixStorage local;
    GteMatrix world;
    u16 *firstColumn;
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
    u32 last;
    const GteMatrixWords *matrix;
    u32 first; /* word 0 is read before the rest of the copy */

    if (actor->flags & 0x400)
        return 0;
    if (!actor->visible)
        return 0;

    radius = actor->header->shadow_radius;
    origin.x = actor->x.integer;
    origin.y = actor->y.integer;
    origin.z = actor->z.integer;
    source = &actor->matrices[actor->shadow_matrix_index];
    first = source->words[0];
    local.words[1] = source->words[1];
    local.words[2] = source->words[2];
    local.words[3] = source->words[3];
    local.words[4] = source->words[4];
    local.words[5] = source->words[5];
    local.words[6] = source->words[6];
    last = source->words[7];
    corner.z = 0x1000;
    asm volatile("" : : "m"(corner.z), "r"(last));
    matrix = (const GteMatrixWords *)local.words;
    local.words[0] = first;
    corner.x = 0;
    corner.y = 0;
    local.matrix.t[1] = 0;
    local.matrix.t[0] = 0;
    /* Retain the final word of the retail copy before clearing translation. */
    *(volatile u32 *)&local.words[7] = last;
    local.matrix.t[2] = 0;
    {
        asm volatile("" : "=r"(matrix) : "0"(matrix));
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
        a = matrix->tx;
        b = matrix->ty;
        gte_ctc2_5(a);
        c = matrix->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    gte_lwc2_0_0(&corner);
    gte_lwc2_1_4(&corner);
    gte_rt();
    gte_swc2_25_0(&axis);
    gte_swc2_26_4(&axis);
    gte_swc2_27_8(&axis);
    axis.y = 0;
    if (axis.x == 0 && axis.z == 0) {
        normal.x = 0;
        normal.y = 0;
        normal.z = 0x1000;
    } else {
        VectorNormal(&axis, &normal);
    }
    normalX = normal.x;
    normalY = normal.y;
    normalZ = normal.z;
    local.matrix.m[1][1] = 0x1000;
    up.y = 0x1000;
    asm volatile("" : : "r"(normalX), "r"(normalY), "r"(normalZ), "m"(up.y));
    local.matrix.m[0][1] = 0;
    local.matrix.m[2][1] = 0;
    up.x = 0;
    up.z = 0;
    local.matrix.m[0][2] = normalX;
    local.matrix.m[1][2] = normalY;
    local.matrix.m[2][2] = normalZ;
    {
        const GteVector *vector = &normal;
        asm volatile("" : "=r"(vector) : "0"(vector));
        a = vector->x;
        b = vector->y;
        gte_ctc2_0(a);
        c = vector->z;
        gte_ctc2_2(b);
        gte_ctc2_4(c);
    }
    gte_ldir3_precise(&up);
    gte_ldir1_precise(&up);
    gte_ldir2_precise(&up);
    gte_op12_psyq();
    gte_swc2_25_0(&axis);
    gte_swc2_26_4(&axis);
    gte_swc2_27_8(&axis);
    local.matrix.m[0][0] = axis.x;
    local.matrix.m[1][0] = axis.y;
    local.matrix.m[2][0] = axis.z;
    if (actor->flags & 0x4000000)
        local.matrix.t[1] = D_800942EC;
    else
        local.matrix.t[1] = origin.y;
    local.matrix.t[0] = actor->matrices[actor->shadow_matrix_index].matrix.t[0];
    local.matrix.t[2] = actor->matrices[actor->shadow_matrix_index].matrix.t[2];
    {
        const GteMatrixWords *cameraRot;
        const GteMatrixWords *cameraTrans;
        const u16 *column;
        u16 *outColumn;

        s32 *outTranslation;
        const s32 *translation;
        cameraRot = (const GteMatrixWords *)D_800B89F8;
        /* Keep the camera address in t0 without pinning it: GCC must also
         * be able to reuse t0 for the signed division below. */
        asm volatile("" : : : "$3", "$4", "$5", "$6", "$7");
        asm volatile("" : "=r"(cameraRot) : "0"(cameraRot));

        a = cameraRot->r11_r12;
        b = cameraRot->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = cameraRot->r22_r23;
        b = cameraRot->r31_r32;
        c = cameraRot->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        column = (const u16 *)&local.matrix;
        asm volatile("" : "=r"(column) : "0"(column));
        a = column[0];
        b = column[3];
        c = column[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        firstColumn = (u16 *)&world;
        asm volatile("" : "=r"(firstColumn) : "0"(firstColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        firstColumn[0] = a;
        firstColumn[3] = b;
        firstColumn[6] = c;
        asm volatile("" : : : "memory");
        column = (const u16 *)&local.matrix + 1;
        asm volatile("" : "=r"(column) : "0"(column));
        a = column[0];
        b = column[3];
        c = column[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&world + 1;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        outColumn[0] = a;
        outColumn[3] = b;
        outColumn[6] = c;
        asm volatile("" : : : "memory");
        column = (const u16 *)&local.matrix + 2;
        asm volatile("" : "=r"(column) : "0"(column));
        a = column[0];
        b = column[3];
        c = column[6];
        gte_mtc2_9(a);
        gte_mtc2_10(b);
        gte_mtc2_11(c);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_ir_sf12();
        outColumn = (u16 *)&world + 2;
        asm volatile("" : "=r"(outColumn) : "0"(outColumn));
        gte_mfc2_9(a);
        gte_mfc2_10(b);
        gte_mfc2_11(c);
        outColumn[0] = a;
        outColumn[3] = b;
        outColumn[6] = c;
        /* Transform placement translation with the camera matrix. */
        asm volatile("" : : : "memory");
        cameraTrans = cameraRot;

        a = cameraTrans->tx;
        b = cameraTrans->ty;
        gte_ctc2_5(a);
        c = cameraTrans->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
        translation = local.matrix.t;
        asm volatile("" : "=r"(translation) : "0"(translation));
        b = ((const u16 *)translation)[2];
        a = ((const u16 *)translation)[0];
        b <<= 16;
        a |= b;
        gte_mtc2_0(a);
        gte_lwc2_1_8(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        outTranslation = world.t;
        gte_swc2_9_0(outTranslation);
        gte_swc2_10_4(outTranslation);
        gte_swc2_11_8(outTranslation);
    }
    SetRotMatrix((GteMatrix *)firstColumn);
    SetTransMatrix((GteMatrix *)firstColumn);
    SetGeomScreen(D_800B89F8[8]);

    /* Exclude unused temporaries from GCC's division reload scratch choice. */
    asm volatile("" : : : "$9", "$10", "$11", "$15", "$24", "$25");
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
    /* These registers are already saved by the function. Excluding them
     * from reload leaves t0 available for the multiply-high temporary. */
    asm volatile("" : : : "$17", "$18", "$19", "$20", "$21", "$22", "$23");
    return 0;
}

void Render_SetPrimColour(PrimObj *obj, unsigned char a, unsigned char b, unsigned char c) {
    int i;
    int offset;
    unsigned char *base;

    for (i = 0; i < obj->count; i++) {
        offset = i << 4;
        base = (unsigned char *) obj->entries;
        base[offset + 4] = a;
        base[offset + 5] = b;
        base[offset + 6] = c;

        base = (unsigned char *) obj->entries + (obj->count << 4);
        base[offset + 4] = a;
        base[offset + 5] = b;
        base[offset + 6] = c;
    }
}
