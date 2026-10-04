#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_pulled_sprite.h"

void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y,
                   int texture, int clut, int page, int intensity,
                   RenderColor *color, int pull)
{
    GteShortVector center;
    GteShortVector screen;
    s32 depth;
    FieldStripPacket *packet;
    int r, g, b;
    int u, v;
    int width, height;

    center.x = 0xA0;
    center.y = 0x78;
    packet = (FieldStripPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldStripPacket);
    if (color == 0) {
        r = g = b = intensity;
    } else {
        r = intensity * color->r / 128;
        g = intensity * color->g / 128;
        b = intensity * color->b / 128;
    }
    packet->r0 = r;
    packet->g0 = g;
    packet->b0 = b;
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
    gte_ldrotmatrix(D_800BCFA4.value);
    gte_ldtransmatrix(D_800BCFA4.value);
    gte_ldv0(position);
    gte_rtps();
    packet->tag.length = 9;
    packet->code = 0x2C;
    if (page == 0xFF) {
        packet->code &= ~2;
        packet->tpage = D_800F3368.tpage;
    } else {
        packet->code |= 2;
        packet->tpage = D_800F3368.tpage | GetTPage(0, page, 0, 0);
    }
    packet->clut = clut;
    width = D_800F3368.extent_x;
    height = D_800F3368.extent_y;
    packet->u0 = packet->u2 = u;
    packet->v1 = packet->v0 = v;
    packet->u1 = packet->u3 = u + width - 1;
    packet->v2 = packet->v3 = v + height - 1;
    width = width * scale_x / 8192;
    height = height * scale_y / 8192;
    gte_stszotz(&depth);
    depth -= (u16)D_800F3368.depth;
    if (pull != 0) {
        depth = 14;
    }
    if ((u32)depth < 0x1000) {
        gte_stsxy2(&screen);
        pull *= 2;
        LoadAverageShort12(&screen, &center, 0x1000 - pull, pull, &screen);
        packet->x0 = packet->x2 = screen.x - width;
        packet->y0 = packet->y1 = screen.y - height;
        packet->x1 = packet->x3 = screen.x + width;
        packet->y2 = packet->y3 = screen.y + height;
        AddPrim(STRIP_OT(depth), packet);
    }
}
