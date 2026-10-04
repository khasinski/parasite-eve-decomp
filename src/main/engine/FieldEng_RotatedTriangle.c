#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_rotated_triangle.h"

/* Triangle with its apex at `position` and a `breadth` wide base `length`
 * away along local Z, turned by `rotation` and shaded per corner from
 * color0..color2 at intensity/128; mode 0xFF draws it opaque, anything
 * else adds a semi-transparent draw mode. */
void func_800D0E88(GteShortVector *position, GteRotation *rotation, int length,
                   int breadth, RenderColor *color0, RenderColor *color1,
                   RenderColor *color2, int intensity, int mode)
{
    GteShortVector corners[3];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldG3Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    GteMatrix *view;
    int bias;

    black = D_800C22A0;
    bias = (u16)D_800F3374;
    packet = (FieldG3Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldG3Packet);
    view = D_800BCFA4.value;
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    gte_ldv0(position);
    gte_rt();
    corners[1].x = -breadth / 2;
    corners[1].y = corners[2].y = 0;
    corners[0].x = corners[0].y = corners[0].z = 0;
    corners[1].z = corners[2].z = length;
    corners[2].x = breadth / 2;
    gte_stmac(matrix.t);
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    gte_ldrotmatrix(&matrix);
    gte_ldtransmatrix(&matrix);
    gte_ldv3(&corners[0], &corners[1], &corners[2]);
    gte_rtpt_padded();
    SetPolyG3(packet);
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = color0->r * (s16)intensity / 128;
    packet->g0 = color0->g * (s16)intensity / 128;
    packet->b0 = color0->b * (s16)intensity / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r1 = color1->r * (s16)intensity / 128;
    packet->g1 = color1->g * (s16)intensity / 128;
    packet->b1 = color1->b * (s16)intensity / 128;
    if (color2 == 0) {
        color2 = &black;
    }
    packet->r2 = color2->r * (s16)intensity / 128;
    packet->g2 = color2->g * (s16)intensity / 128;
    packet->b2 = color2->b * (s16)intensity / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_avsz3_padded();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth < 0x1000) {
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
}
