#include "pe1/gte.h"
#include "pe1/render_object.h"
#include "pe1/field_tile.h"

/* Haloed point at distance `x` from the view origin rotated by `y` about Z.
 * The halo is always drawn semi-transparent; `mode` is not read. */
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
    gte_ldrotmatrix(&matrix);
    gte_ldv0(&offset);
    gte_rtps();
    SetTile1(point);
    SetTile(glow);
    Gpu_SetDither(point, 1);
    glow->r = point->r = rgb[0] * scale / 128;
    glow->g = point->g = rgb[1] * scale / 128;
    glow->b = point->b = rgb[2] * scale / 128;
    glow->r >>= 2;
    glow->g >>= 2;
    glow->b >>= 2;
    gte_stsxy2(&point->x);
    gte_stszotz(&depth);
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
        SetDrawMode((char *)blend, 0, 1, GetTPage(0, 1, 0, 0));
        if (glow) {
            glow->command |= 2;
            TILE_OT_ADDPRIM(ot, glow, link);
        }
        TILE_OT_ADDPRIM(ot, blend, link);
    }
}
