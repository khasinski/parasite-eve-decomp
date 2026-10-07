#include "pe1/gte.h"
#include "pe1/render_object.h"
#include "pe1/field_tile.h"

/* Haloed point at distance `x` from the view origin rotated by `y` about Z.
 * The halo is always drawn semi-transparent; `mode` is not read.
 * Matching debt: two pins and one empty pointer constraint. The rotation
 * upload is gte_ldrotmatrix. */
void func_800D27FC(int x, int y, void *color, int scale, int mode)
{
    GteShortVector offset;
    GteMatrix matrix;
    FieldTilePoint *point;
    RenderTintTile *glow;
    RenderTintMode *blend;
    u8 *rgb = color;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    memset(&offset, 0, sizeof(offset));
    offset.x = x;
    offset.y = 0;
    offset.z = 0;
    point = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    glow = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x1000;
    matrix.m[0][1] = matrix.m[0][2] = matrix.m[1][0] = matrix.m[1][2] =
        matrix.m[2][0] = matrix.m[2][1] = 0;
    RotMatrixZ(y, &matrix);
    {
        const GteMatrixWords *words = (const GteMatrixWords *)&matrix;
        gte_ldrotmatrix(words);
    }
    gte_lwc2_0_0(&offset);
    gte_lwc2_1_4(&offset);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(point);
    SetTile(glow);
    SetSemiTrans(point, 1);
    glow->r = point->r = rgb[0] * scale / 128;
    glow->g = point->g = rgb[1] * scale / 128;
    glow->b = point->b = rgb[2] * scale / 128;
    glow->r >>= 2;
    glow->g >>= 2;
    glow->b >>= 2;
    gte_stsxy2(&point->x);
    {
        register s32 z asm("$12");
        s32 *out = (s32 *)&depth;
        asm volatile("" : "=r"(out) : "0"(out));
        gte_getsz3(z);
        gte_cop2_hazard_slot();
        z >>= 2;
        *out = z;
    }
    depth -= (u16)D_800F3374;
    if (depth < 0x1000) {
        glow->x = point->x - 1;
        glow->y = point->y - 1;
        glow->height = 3;
        glow->width = 3;
        AddPrim((u32 *)D_800B0E38.ordering[D_8009CDDC] + depth, (u32 *)point);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        blend = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
        D_8009CDD8 += sizeof(RenderTintMode);
        SetDrawTPage((char *)blend, 0, 1, GetTPage(0, 1, 0, 0));
        if (glow) {
            glow->command |= 2;
            TILE_OT_ADDPRIM(ot, glow, link);
        }
        TILE_OT_ADDPRIM(ot, blend, link);
    }
}
