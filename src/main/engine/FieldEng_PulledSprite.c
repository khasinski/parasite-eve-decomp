#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_pulled_sprite.h"

/* Matching debt: six pins, four empty barriers and five volatile stack
 * parameters. The volatile copies and entry memory barrier keep all register
 * saves before the ordered argument loads. Each argument is read exactly once.
 * Matrix loads and depth scaling/stores are C;
 GTE ops and nops are individual. */
void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y,
                   int texture, volatile int input_clut, volatile int input_page,
                   volatile int input_intensity, RenderColor *volatile input_color,
                   volatile int input_pull)
{
    GteShortVector center;
    GteShortVector screen;
    s32 depth;
    FieldStripPacket *packet;
    int r, g, b;
    int u, v;
    int width, height;
    register int clut asm("$19");
    int page;
    int intensity;
    register RenderColor *color;
    int pull;

    asm volatile("" : : : "memory");
    pull = input_pull;
    clut = input_clut;
    page = input_page;
    intensity = input_intensity;
    color = input_color;
    asm volatile("" : : : "$6");
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
    {
        register RenderMatrixSlot *slot = &D_800BCFA4;
        register const u32 *words asm("$11");
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        asm volatile("" : "=r"(slot) : "0"(slot));
        words = (u32 *)slot->value;

        a = words[0];
        b = words[1];
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = words[2];
        b = words[3];
        c = words[4];
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = words[5];
        b = words[6];
        gte_ctc2_5(a);
        c = words[7];
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
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
    {
        register s32 z asm("$12");
        s32 *depthOut = &depth;
        asm volatile("" : "=r"(depthOut) : "0"(depthOut));
        gte_getsz3(z);
        gte_cop2_hazard_slot();
        z >>= 2;
        *depthOut = z;

    }
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
