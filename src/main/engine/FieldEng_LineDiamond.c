#include "common.h"
#include "pe1/gte.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"

void func_800DB25C(GteShortVector *position, int x, int y, int angle,
                   int radius, int depth, int intensity, RenderColor *color)
{
    GteShortVector center, p0, p1, p2, p3;
    RenderLinePacket *line;
    int r, g, b, dx, dy;
    line = (RenderLinePacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += 64;
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    PE1_NOP();
    PE1_NOP();
    gte_rtps_command();
    r = color->r * intensity / 128;
    g = color->g * intensity / 128;
    b = color->b * intensity / 128;
    gte_stsxy2(&center);
    center.x += x;
    center.y += y;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p0.x = center.x + dx;
    p0.y = center.y + dy;
    p2.x = center.x - dx;
    p2.y = center.y - dy;
    angle += 1024;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p1.x = center.x + dx;
    p1.y = center.y + dy;
    p3.x = center.x - dx;
    p3.y = center.y - dy;
#define EDGE(a, z) \
    SetLineF2(line); \
    line->r = r; line->g = g; line->b = b; \
    line->x0 = a.x; line->y0 = a.y; \
    line->x1 = z.x; line->y1 = z.y; \
    func_800CF6F8(D_800B0E38.ordering[D_8009CDDC] + 16, line, depth)
    EDGE(p0, p1);
    line++;
    EDGE(p1, p2);
    line++;
    EDGE(p2, p3);
    line++;
    EDGE(p3, p0);
#undef EDGE
}
