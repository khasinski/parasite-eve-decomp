#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_rotated_quad.h"

/* Rectangle `length` deep and `breadth` wide in the local XZ plane at
 * `position`, turned by `rotation`, gouraud shaded from color0 (near edge)
 * to color1 (far edge) at intensity/128 and textured with the cell at
 * (u, v); mode 0xFF draws it opaque, anything else semi-transparent. */
void func_800D2370(GteShortVector *position, GteRotation *rotation,
                   int length, int breadth, int u, int v, int texture_width,
                   int texture_height, int clut, RenderColor *color0,
                   RenderColor *color1, int intensity, int mode)
{
    GteShortVector corners[4];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldGt4Packet *packet;
    s16 scale;
    int bias;
    GteMatrix *view;

    view = D_800BCFA4.value;
    scale = intensity;
    bias = (u16)D_800F3374;
    black = D_800C22A0;
    packet = (FieldGt4Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldGt4Packet);
    SetPolyGT4(packet);
    if (mode == 0xFF) {
        packet->code &= ~2;
        packet->tpage = D_800F3370;
    } else {
        packet->code |= 2;
        packet->tpage = D_800F3370 | GetTPage(0, mode, 0, 0);
    }
    packet->clut = clut;
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    gte_ldv0(position);
    gte_rt();
    breadth /= 2;
    corners[0].x = corners[2].x = -breadth;
    corners[1].x = corners[3].x = breadth;
    corners[2].z = corners[3].z = length;
    corners[0].y = corners[1].y = corners[2].y = corners[3].y = 0;
    corners[0].z = corners[1].z = 0;
    gte_stmac(matrix.t);
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    gte_ldrotmatrix(&matrix);
    gte_ldtransmatrix(&matrix);
    gte_ldv3(&corners[0], &corners[1], &corners[2]);
    gte_rtpt_padded();
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = packet->r1 = color0->r * scale / 128;
    packet->g0 = packet->g1 = color0->g * scale / 128;
    packet->b0 = packet->b1 = color0->b * scale / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r2 = packet->r3 = color1->r * scale / 128;
    packet->g2 = packet->g3 = color1->g * scale / 128;
    packet->b2 = packet->b3 = color1->b * scale / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_avsz3_padded();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth < 0x1000) {
            gte_ldv0(&corners[3]);
            gte_rtps();
            packet->u0 = packet->u1 = u;
            packet->u2 = packet->u3 = u + texture_width - 1;
            packet->v0 = packet->v2 = v;
            packet->v1 = packet->v3 = v + texture_height - 1;
            gte_stsxy2(&packet->x3);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth,
                    (unsigned int *)packet);
        }
    }
}
