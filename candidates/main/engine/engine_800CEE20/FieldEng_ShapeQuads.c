#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_shape_quads.h"

void func_800CEE20(GteShortVector *position, GteRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RenderColor *color)
{
    FieldStripPacket template;
    GteVector scale;
    GteRotation level;
    GteMatrix matrix;
    s32 depth;
    FieldStripPacket *packet;
    s32 *view;
    GteShortVector *vertex;
    int r, g, b;
    int u, v;
    int width, height;
    int bias;
    int i;

    level = D_800C2268;
    if (rotation == 0) {
        rotation = &level;
    }
    template.tag.length = 9;
    template.code = 0x2C;
    if (page == 0xFF) {
        template.code &= ~2;
        template.tpage = D_800F3368.tpage;
    } else {
        template.code |= 2;
        width = GetTPage(0, page, 0, 0);
        template.tpage = D_800F3368.tpage | width;
    }
    view = D_800BCFA4.value;
    template.clut = clut;
    if (color == 0) {
        r = intensity;
        g = r;
        b = r;
    } else {
        r = intensity * color->r / 128;
        g = intensity * color->g / 128;
        b = intensity * color->b / 128;
    }
    template.r0 = r;
    template.g0 = g;
    template.b0 = b;
    if (D_800F3368.parameter06 != 0) {
        u = texture & 0xF;
        v = 0;
        if (u >= 8) {
            u -= 8;
            v = 0x20;
        }
        u <<= 4;
        v += texture / 16 * 16;
    } else {
        u = (texture & 0xF) << 4;
        v = texture / 16 * 16;
    }
    if (D_800F3368.palette == 4 && D_800F3428 != 0) {
        v += 0x60;
    }
    width = D_800F3368.extent_x;
    height = D_800F3368.extent_y;
    scale.x = scale_x * (width >> 4);
    template.u0 = template.u2 = u;
    template.v1 = template.v0 = v;
    scale.y = scale_y * (height >> 4);
    template.v2 = template.v3 = v + height - 1;
    scale.z = 0x1000;
    template.u1 = template.u3 = u + width - 1;
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    gte_ldv0(position);
    gte_rt();
    packet = (FieldStripPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += D_800E1210[D_800F3368.parameter0A] * sizeof(FieldStripPacket);
    bias = (u16)D_800F3368.depth;
    RotMatrixYXZ(rotation, &matrix);
    Gte_ScaleMatrix(&matrix, &scale);
    gte_swc2_25_0(matrix.t);
    gte_swc2_26_4(matrix.t);
    gte_swc2_27_8(matrix.t);
    if (rotation->flags == 1) {
        MulRotMatrix(&matrix);
    }
    gte_ldrotmatrix(&matrix);
    gte_ldtransmatrix(&matrix);
    vertex = D_800E13BC[D_800F3368.parameter0A];
    for (i = 0; i < D_800E1210[D_800F3368.parameter0A]; i++, vertex += 4, packet++) {
        gte_ldv3(&vertex[0], &vertex[1], &vertex[2]);
        gte_rtpt_padded();
        *packet = template;
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_stmac0(&depth);
        if (depth == 0) {
            break;
        }
        gte_stszotz(&depth);
        gte_ldv0(&vertex[3]);
        gte_rtps();
        depth -= bias;
        if ((u32)depth < 0x1000) {
            FieldStripLink link;

            gte_stsxy2(&packet->x3);
            packet->tag.address = STRIP_OT(depth)->address;
            link.tag = &packet->tag;
            STRIP_OT(depth)->address = link.word;
        }
    }
}
