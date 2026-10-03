#include "pe1/field_tile.h"

/* Full-screen 320x240 tile in `color` scaled by scale/128, linked at
 * `depth`; any blend mode other than 0xFF adds a semi-transparent draw mode. */
void func_800D1AE0(u8 *color, int scale, int abr, u32 depth)
{
    RenderTintTile *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;

    tile = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    SetTile(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    if (depth < 0x1000) {
        tile->x = 0;
        tile->y = 0;
        tile->width = 320;
        tile->height = 240;
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
