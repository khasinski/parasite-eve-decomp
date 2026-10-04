/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_ring_band.h"

/* `segments` quads around `position` between radius `inner` (shaded by
 * color0) and radius `outer` (shaded by color1) at intensity/128; a
 * nonzero rotation->flags also applies the view rotation. Drawing stops at
 * the first quad beyond the depth range. */
void func_800D0728(GteShortVector *position, int inner, int outer, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   struct RenderColor *color0, struct RenderColor *color1,
                   int intensity, int mode)
{
    FieldG4Packet template;
    GteShortVector vertices[4];
    GteShortVector anchor;
    GteShortVector defaultRotation = D_800C2260;
    GteVector scale = D_800C2290;
    RenderColor innerColor;
    RenderColor outerColor;
    GteMatrix matrix;
    u32 depth;
    GteMatrix *view;
    FieldG4Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    RenderGpuTag *entry;
    int bias;
    int i;
    int angle;
    int next;

    if (segments < 4)
        return;
    bias = (u16)D_800F3374;
    view = (GteMatrix *)D_800BCFA4.value;
    if (color0 == 0) {
        innerColor.r = innerColor.g = innerColor.b = 0;
    } else {
        innerColor.r = color0->r * intensity / 128;
        innerColor.g = color0->g * intensity / 128;
        innerColor.b = color0->b * intensity / 128;
    }
    if (color1 == 0) {
        outerColor.r = outerColor.g = outerColor.b = 0;
    } else {
        outerColor.r = color1->r * intensity / 128;
        outerColor.g = color1->g * intensity / 128;
        outerColor.b = color1->b * intensity / 128;
    }
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    anchor.x = position->x;
    anchor.y = position->y;
    anchor.z = position->z;
    gte_ldv0(&anchor);
    gte_rt();
    if (rotation == 0)
        rotation = (GteRotation *)&defaultRotation;
    scale.x = scale_x;
    scale.y = scale_y;
    gte_stmac(matrix.t);
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    if (rotation->flags)
        MulRotMatrix(&matrix);
    Gte_ScaleMatrix(&matrix, &scale);
    gte_ldrotmatrix(&matrix);
    gte_ldtransmatrix(&matrix);
    SetPolyG4(&template);
    template.r1 = template.r0 = outerColor.r;
    template.g1 = template.g0 = outerColor.g;
    template.b1 = template.b0 = outerColor.b;
    template.r2 = innerColor.r;
    template.g2 = innerColor.g;
    template.b2 = innerColor.b;
    template.r3 = innerColor.r;
    template.g3 = innerColor.g;
    template.b3 = innerColor.b;
    packet = (FieldG4Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += segments * sizeof(FieldG4Packet);
    vertices[0].z = vertices[1].z = vertices[2].z = vertices[3].z = 0;
    for (i = 0; i < segments; i++, packet++) {
        angle = (i << 12) / segments;
        next = ((i + 1) << 12) / segments;
        vertices[0].x = rcos(angle) * outer / 4096;
        vertices[0].y = rsin(angle) * outer / 4096;
        vertices[1].x = rcos(next) * outer / 4096;
        vertices[1].y = rsin(next) * outer / 4096;
        vertices[2].x = rcos(angle) * inner / 4096;
        vertices[2].y = rsin(angle) * inner / 4096;
        vertices[3].x = rcos(next) * inner / 4096;
        vertices[3].y = rsin(next) * inner / 4096;
        gte_ldv3(&vertices[0], &vertices[1], &vertices[2]);
        gte_rtpt_padded();
        *packet = template;
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_avsz3_padded();
        gte_stszotz(&depth);
        gte_ldv0(&vertices[3]);
        gte_rtps();
        depth -= bias;
        if (depth >= 0x1000)
            return;
        gte_stsxy2(&packet->x3);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        entry = ot.tag;
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawMode((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
            if (packet) {
                packet->code |= 2;
                RING_OT_ADDPRIM(entry, packet, link);
            }
            RING_OT_ADDPRIM(entry, drawMode, link);
        } else if (packet) {
            RING_OT_ADDPRIM(entry, packet, link);
        }
    }
}
