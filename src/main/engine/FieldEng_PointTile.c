#include "pe1/gte.h"
#include "pe1/field_tile.h"

/* Projected one-pixel tile plus a 3x3 halo at a quarter of its colour. */
void func_800D1DEC(GteShortVector *position, u8 *color, int scale, int abr)
{
    FieldTilePoint *point;
    RenderTintTile *glow;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    point = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    glow = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    gte_ldv0(position);
    gte_rtps();
    SetTile1(point);
    Gpu_SetDither(point, 1);
    SetTile(glow);
    glow->r = point->r = color[0] * scale / 128;
    glow->g = point->g = color[1] * scale / 128;
    glow->b = point->b = color[2] * scale / 128;
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
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawMode((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (glow) {
                glow->command |= 2;
                TILE_OT_ADDPRIM(ot, glow, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (glow) {
            TILE_OT_ADDPRIM(ot, glow, link);
        }
    }
}

/* One-pixel tile at the projected `position` in `color` scaled by
 * scale/128, linked at its view depth; any blend mode other than 0xFF adds a
 * semi-transparent draw mode. */
void func_800D2104(GteShortVector *position, u8 *color, int scale, int abr)
{
    FieldTilePoint *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    tile = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    gte_ldv0(position);
    gte_rtps();
    SetTile1(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    gte_stsxy2(&tile->x);
    gte_stszotz(&depth);
    depth -= (u16)D_800F3374;
    if (depth < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawMode((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (tile) {
                tile->command |= 2;
                TILE_OT_ADDPRIM(ot, tile, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (tile) {
            TILE_OT_ADDPRIM(ot, tile, link);
        }
    }
}
