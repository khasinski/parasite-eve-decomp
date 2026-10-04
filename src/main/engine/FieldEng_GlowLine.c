#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_glow_line.h"

/* Gouraud line from `from` (color0 at scale0/128) to `to` (color1 at
 * scale1/128) linked at the mean depth, with a 16x16 glow cell stretched
 * along it; mode 0xFF draws both opaque, anything else semi-transparent. */
void func_800D2B58(GteShortVector *from, GteShortVector *to, u8 *color0,
                   u8 *color1, int scale0, int scale1, int mode)
{
    u32 depth[2];
    FieldLineG2Packet *line;
    FieldGlowQuadPacket *glow;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int angle;
    int across;
    int acrossY;
    int along;
    int alongY;

    line = (FieldLineG2Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldLineG2Packet);
    glow = (FieldGlowQuadPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldGlowQuadPacket);
    gte_ldv0(from);
    gte_rtps();
    line->r0 = color0[0] * scale0 / 128;
    line->g0 = color0[1] * scale0 / 128;
    line->b0 = color0[2] * scale0 / 128;
    gte_stszotz(&depth[0]);
    gte_stsxy2(&line->x0);
    line->tag.length = 4;
    line->code = 0x50;
    gte_ldv0(to);
    gte_rtps();
    line->r1 = color1[0] * scale1 / 128;
    line->g1 = color1[1] * scale1 / 128;
    line->b1 = color1[2] * scale0 / 128;
    gte_stszotz(&depth[1]);
    gte_stsxy2(&line->x1);
    depth[0] = (int)(depth[0] + depth[1]) / 2 - (u16)D_800F3374;
    if (depth[0] < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth[0]);
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawMode((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
            if (line) {
                line->code |= 2;
                TILE_OT_ADDPRIM(ot, line, link);
            }
            TILE_OT_ADDPRIM(ot, drawMode, link);
        } else if (line) {
            TILE_OT_ADDPRIM(ot, line, link);
        }
        angle = Gte_Atan2(line->y1 - line->y0, line->x1 - line->x0);
        across = rsin(-angle) / 512;
        acrossY = rcos(-angle) / 512;
        along = rcos(angle) / 256;
        alongY = rsin(angle) / 256;
        glow->tag.length = 12;
        glow->code = 0x3C;
        if (mode == 0xFF) {
            glow->code = 0x3C;
            glow->tpage = D_800E2852;
        } else {
            glow->code = 0x3E;
            glow->tpage = D_800E2852 | GetTPage(0, mode, 0, 0);
        }
        glow->clut = GetClut(0x20, 0x1D6);
        glow->x0 = line->x0 - along + across;
        glow->y0 = line->y0 - alongY + acrossY;
        glow->x1 = line->x0 - along - across;
        glow->y1 = line->y0 - alongY - acrossY;
        glow->x2 = line->x1 + along + across;
        glow->y2 = line->y1 + alongY + acrossY;
        glow->x3 = line->x1 + along - across;
        glow->y3 = line->y1 + alongY - acrossY;
        glow->u0 = 0x90;
        glow->u2 = 0x90;
        glow->v0 = 0xD0;
        glow->v1 = 0xD0;
        glow->u1 = 0x9F;
        glow->u3 = 0x9F;
        glow->v2 = 0xDF;
        glow->v3 = 0xDF;
        glow->r0 = line->r0;
        glow->g0 = line->g0;
        glow->b0 = line->b0;
        glow->r1 = line->r0;
        glow->g1 = line->g0;
        glow->b1 = line->b0;
        glow->r2 = line->r1;
        glow->g2 = line->g1;
        glow->b2 = line->b1;
        glow->r3 = line->r1;
        glow->g3 = line->g1;
        glow->b3 = line->b1;
        AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth[0],
                (unsigned int *)glow);
    }
}
