/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_textured_ribbon.h"

/* Draw a textured ribbon `width` wide through `count` points: every joint
 * is widened across the screen direction of the segment that leaves it,
 * shaded from color0 to color1 (each at scale/128) along the ribbon and
 * textured from u..u+du by v..v+height. Mode 0xFF draws opaque, anything
 * else semi-transparent. */
void func_800D3114(GteShortVector *points, s16 count, int width, int u, int v,
                   int du, int height, int tpage, int clut, int scale,
                   RenderColor *color0, RenderColor *color1, int mode)
{
    GteShortVector *point;
    GteShortVector angles;
    GteShortVector right;
    GteShortVector left;
    FieldRibbonColor color0Scaled;
    FieldRibbonColor color1Scaled;
    FieldRibbonColor color;
    FieldScreenPoint screen0;
    FieldScreenPoint screen1;
    GteMatrix matrix;
    u32 unused;
    u32 depth;
    FieldRibbonPacket *packet;
    int i;
    u32 previousDepth;
    int bias;
    int t;
    u32 shade;
    int dx;
    int dy;
    int angle;
    int joint;

    packet = (FieldRibbonPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += count * sizeof(FieldRibbonPacket);
    bias = (u16)D_800F3374;
    color0Scaled.bytes.rgb[0] = color0->r * scale / 128;
    color0Scaled.bytes.rgb[1] = color0->g * scale / 128;
    color0Scaled.bytes.rgb[2] = color0->b * scale / 128;
    color1Scaled.bytes.rgb[0] = color1->r * scale / 128;
    color1Scaled.bytes.rgb[1] = color1->g * scale / 128;
    color1Scaled.bytes.rgb[2] = color1->b * scale / 128;
    right.x = width / 2;
    previousDepth = 0;
    right.y = right.z = 0;
    left.y = left.z = 0;
    left.x = -width / 2;
    angles.x = angles.y = 0;
    point = points;
    for (i = 0; i < count; i++) {
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        gte_ldv3(&point[0], &point[1], &point[0]);
        gte_rtpt_padded();
        packet->tag.length = 12;
        packet->c0.bytes.code = 0x3C;
        if (mode == 0xFF) {
            packet->c0.bytes.code = 0x3C;
            packet->tpage = tpage;
        } else {
            packet->c0.bytes.code = 0x3E;
            packet->tpage = GetTPage(0, mode, 0, 0) | tpage;
        }
        packet->clut = clut;
        gte_stsxy3(&screen0, &screen1, &unused);
        gte_avsz3_padded();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth >= 0x1000) {
            return;
        }
        dx = screen1.x - screen0.x;
        dy = screen1.y - screen0.y;
        angle = Gte_Atan2(dy, dx) + 0x400;
        t = (i << 12) / count;
        LoadAverageCol(&color0Scaled, &color1Scaled, 0x1000 - t, t, &color);
        if (i == 0) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet->u0 = packet->u1 = u;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            gte_stsxy3(&packet->p0, &packet->p1, &unused);
            packet++;
        } else if (i < count - 1) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / count;
            gte_stsxy3(&packet[-1].p2, &packet[-1].p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            packet++;
        } else {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / count;
            gte_stsxy3(&packet[-1].p2, &packet[-1].p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            gte_ldv0(&point[1]);
            gte_rt();
            packet->c2.word = packet->c3.word = color1Scaled.word;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet->v2 = v;
            packet->u2 = packet->u3 = u + du;
            packet->v3 = v + height;
            gte_stsxy3(&packet->p2, &packet->p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth,
                    (unsigned int *)packet);
        }
        point++;
    }
}
