#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/gte.h"

void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y, int cell,
                   int clut, int abr, int intensity, RenderColor *color, int blend)
{
    s16 center[2];
    s16 screen[2];
    int depth;
    RenderTexturedQuad *quad;
    int r, g, b;
    int u, v;
    int width, height;

    center[0] = 160;
    center[1] = 120;
    quad = (RenderTexturedQuad *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += 40;
    if (color == 0) {
        r = g = b = intensity;
    } else {
        r = intensity * color->r / 128;
        g = intensity * color->g / 128;
        b = intensity * color->b / 128;
    }
    quad->color.bytes.r = r;
    quad->color.bytes.g = g;
    quad->color.bytes.b = b;
    if (D_800F3368.parameter06) {
        u = cell & 15;
        v = 0;
        if (u >= 8) {
            u -= 8;
            v = 32;
        }
        u <<= 4;
        v += (cell / 16) << 4;
    } else {
        u = (cell & 15) << 4;
        v = (cell / 16) << 4;
    }
    gte_ldrotmatrix(D_800BCFA4.value);
    gte_ldtransmatrix(D_800BCFA4.value);
    gte_ldv0(position);
    gte_rtps();
    quad->tag.bytes.length = 9;
    quad->color.bytes.code = 0x2C;
    if (abr == 0xFF) {
        quad->color.bytes.code = 0x2C;
        quad->tpage = D_800F3368.tpage;
    } else {
        quad->color.bytes.code = 0x2E;
        quad->tpage = GetTPage(0, abr, 0, 0) | D_800F3368.tpage;
    }
    quad->clut = clut;
    width = D_800F3368.extent_x;
    height = D_800F3368.extent_y;
    quad->u0 = quad->u2 = u;
    quad->v0 = quad->v1 = v;
    quad->u1 = quad->u3 = u + width - 1;
    quad->v2 = quad->v3 = v + height - 1;
    width = width * scale_x / 8192;
    height = height * scale_y / 8192;
    gte_stszotz(&depth);
    depth -= (u16)D_800F3368.depth;
    if (blend)
        depth = 14;
    if ((u32)depth < 0x1000) {
        gte_stsxy2(screen);
        blend *= 2;
        LoadAverageShort12(screen, center, 0x1000 - blend, blend, screen);
        quad->x0 = quad->x2 = screen[0] - width;
        quad->y0 = quad->y1 = screen[1] - height;
        quad->x1 = quad->x3 = screen[0] + width;
        quad->y2 = quad->y3 = screen[1] + height;
        AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth,
                (unsigned int *)quad);
    }
}
