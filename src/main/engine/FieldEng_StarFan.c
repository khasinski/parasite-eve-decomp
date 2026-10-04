/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_star_fan.h"

/* `segments` triangles around `position`: the rim points alternate between
 * radius `width` and `height`, color0 shades the centre and color1 the
 * rim at intensity/128; a nonzero rotation->flags also applies the view
 * rotation. Drawing stops at the first triangle beyond the depth range. */
void func_800D004C(GteShortVector *position, int width, int height, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   RenderColor *color0, RenderColor *color1, int intensity,
                   int mode)
{
    FieldG3Packet template;
    s16 radii[2];
    GteShortVector vertices[3];
    GteShortVector anchor;
    GteShortVector defaultRotation = D_800C2260;
    GteVector scale = D_800C2290;
    RenderColor centre;
    RenderColor rim;
    GteMatrix matrix;
    u32 depth;
    GteMatrix *view;
    FieldG3Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int bias;
    int i;
    int angle;
    int next;

    if (segments < 4)
        return;
    bias = (u16)D_800F3374;
    view = (GteMatrix *)D_800BCFA4.value;
    if (color0 == 0) {
        centre.r = centre.g = centre.b = 0;
    } else {
        centre.r = color0->r * intensity / 128;
        centre.g = color0->g * intensity / 128;
        centre.b = color0->b * intensity / 128;
    }
    if (color1 == 0) {
        rim.r = rim.g = rim.b = 0;
    } else {
        rim.r = color1->r * intensity / 128;
        rim.g = color1->g * intensity / 128;
        rim.b = color1->b * intensity / 128;
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
    Gte_ScaleMatrix(&matrix, &scale);
    if (rotation->flags)
        MulRotMatrix(&matrix);
    gte_ldrotmatrix(&matrix);
    gte_ldtransmatrix(&matrix);
    SetPolyG3(&template);
    template.r0 = centre.r;
    template.g0 = centre.g;
    template.b0 = centre.b;
    template.r2 = template.r1 = rim.r;
    template.g2 = template.g1 = rim.g;
    template.b2 = template.b1 = rim.b;
    packet = (FieldG3Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += segments * sizeof(FieldG3Packet);
    vertices[0].x = vertices[0].y = vertices[0].z = vertices[1].z = vertices[2].z = 0;
    radii[0] = width;
    radii[1] = height;
    for (i = 0; i < segments; i++, packet++) {
        angle = (i << 12) / segments;
        next = ((i + 1) << 12) / segments;
        vertices[1].x = rcos(angle) * radii[i & 1] / 4096;
        vertices[1].y = rsin(angle) * radii[i & 1] / 4096;
        vertices[2].x = rcos(next) * radii[(i + 1) & 1] / 4096;
        vertices[2].y = rsin(next) * radii[(i + 1) & 1] / 4096;
        gte_ldv3(&vertices[0], &vertices[1], &vertices[2]);
        gte_rtpt_padded();
        *packet = template;
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_avsz3_padded();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth >= 0x1000)
            return;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawMode((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
            if (packet) {
                packet->code |= 2;
                TILE_OT_ADDPRIM(ot, packet, link);
            }
            TILE_OT_ADDPRIM(ot, drawMode, link);
        } else if (packet) {
            TILE_OT_ADDPRIM(ot, packet, link);
        }
    }
}
