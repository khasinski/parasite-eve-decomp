#include "fx_common_motion.h"

#define FX_MODE_PACKET                                          \
    (buffer == &g_FxCommonFrames[0].buffer ? &g_FxCommonDrawModes1[1] \
                                               : &g_FxCommonDrawModes1[0])

/* Fade the effect markers in or out by eight steps and draw, for every
 * enabled effect, a line from its projected position to a two-part label
 * (the selected effect uses the brighter palette). */
s16 func_80193B5C(s16 value)
{
    u16 level;
    u8 half;
    u8 dim;
    FxCommonDrawModePacket *mode;
    FxCommonBuffer *buffer;
    FxCommonAddress address;
    FxCommonMarkerCursor packet;
    GteShortVector rotation;
    GteShortVector position;
    GteShortVector unused;
    s32 screen0;
    s32 screen1;
    s32 screen2;
    s32 depth;
    s32 flag;
    u32 xy;
    int u;
    int x;
    int y;
    int i;

    level = 0;
    if (value > 0) {
        if (value < 0xFF)
            value += 8;
        if (value > 0xFF)
            value = 0xFF;
        level = value;
    }
    if (value < 0) {
        if (value >= -0xFE)
            value -= 8;
        if (value < -0xFF)
            value = -0xFF;
        level = value + 0xFF;
    }

    buffer = D_8019C9C0;
    mode = FX_MODE_PACKET;
    mode->tag = (mode->tag & 0xFF000000)
              | (buffer->allocation[10].packed & 0xFFFFFF);
    address.pointer = FX_MODE_PACKET;
    buffer->allocation[10].packed =
        (buffer->allocation[10].packed & 0xFF000000)
        | (address.word & 0xFFFFFF);
    half = level >> 1;
    dim = level >> 3;

    for (i = 0; i < 10; i++) {
        position.x = g_FxCommonMotionWhole[i].x.whole;
        position.y = g_FxCommonMotionWhole[i].y.whole;
        position.z = g_FxCommonMotionWhole[i].z.whole;
        if (g_FxCommonEffectSetup[i].enabled != 1 || level == 0)
            continue;

        rotation.x = g_FxCommonEffectSetup[i].position[0];
        rotation.y = g_FxCommonEffectSetup[i].position[1];
        rotation.z = g_FxCommonEffectSetup[i].position[2];
        func_80079274(&rotation, &position, &unused, &screen0, &screen1,
                      &screen2, &depth, &flag);

        packet.bytes = D_8019C9C0->data;
        packet.line->tag.bytes.length = 3;
        packet.line->code = 0x40;
        func_80077B04(packet.line, 1);
        packet.line->xy0 = screen0;
        packet.line->xy1 = screen1;
        if (D_8019CC52 == i) {
            packet.line->r = half;
            packet.line->g = half;
            packet.line->b = half;
        } else {
            packet.line->r = dim;
            packet.line->g = dim;
            packet.line->b = dim;
        }
        buffer = D_8019C9C0;
        address.pointer = packet.line;
        packet.line->tag.packed = (packet.line->tag.packed & 0xFF000000)
                         | (buffer->allocation[10].packed & 0xFFFFFF);
        buffer->allocation[10].packed =
            (buffer->allocation[10].packed & 0xFF000000)
            | (address.word & 0xFFFFFF);
        packet.line++;
        buffer->data = packet.bytes;

        xy = screen1;
        u = 0;
        func_80077BE4(packet.quad);
        func_80077B04(packet.quad, 1);
        packet.quad->r0 = half;
        packet.quad->g0 = half;
        packet.quad->b0 = half;
        packet.quad->r1 = half;
        packet.quad->g1 = half;
        packet.quad->b1 = half;
        packet.quad->r2 = half;
        packet.quad->g2 = half;
        packet.quad->b2 = half;
        packet.quad->r3 = half;
        packet.quad->g3 = half;
        packet.quad->b3 = half;
        packet.quad->u0 = u;
        packet.quad->v0 = i * 13;
        packet.quad->u1 = u + 0x7F;
        packet.quad->v1 = i * 13;
        packet.quad->u2 = u;
        packet.quad->v2 = i * 13 + 12;
        packet.quad->u3 = u + 0x7F;
        packet.quad->v3 = i * 13 + 12;
        x = xy;
        y = xy >> 16;
        packet.quad->x0 = x;
        packet.quad->y0 = y;
        packet.quad->x1 = x + 0x7F;
        packet.quad->y1 = y;
        packet.quad->x2 = x;
        packet.quad->y2 = y + 12;
        packet.quad->x3 = x + 0x7F;
        packet.quad->y3 = y + 13;
        if (D_8019CC52 == i)
            packet.quad->clut = func_80077AA4(0x3C0, 0x90);
        else
            packet.quad->clut = func_80077AA4(0x3C0, 0x8F);
        packet.quad->tpage = func_80077A64(0, 1, 0x3C0, 0);
        buffer = D_8019C9C0;
        address.pointer = packet.quad;
        packet.quad->tag.packed = (packet.quad->tag.packed & 0xFF000000)
                         | (buffer->allocation[9].packed & 0xFFFFFF);
        buffer->allocation[9].packed =
            (buffer->allocation[9].packed & 0xFF000000)
            | (address.word & 0xFFFFFF);
        packet.quad++;

        u = 0x80;
        func_80077BE4(packet.quad);
        func_80077B04(packet.quad, 1);
        packet.quad->r0 = u;
        packet.quad->g0 = u;
        packet.quad->b0 = u;
        packet.quad->r1 = u;
        packet.quad->g1 = u;
        packet.quad->b1 = u;
        packet.quad->r2 = u;
        packet.quad->g2 = u;
        packet.quad->b2 = u;
        packet.quad->r3 = u;
        packet.quad->g3 = u;
        packet.quad->b3 = u;
        packet.quad->x0 = x;
        packet.quad->y0 = y;
        packet.quad->x1 = x + 0x7F;
        packet.quad->y1 = y;
        packet.quad->x2 = x;
        packet.quad->y2 = y + 12;
        packet.quad->x3 = x + 0x7F;
        packet.quad->y3 = y + 13;
        packet.quad->u0 = u;
        packet.quad->v0 = i * 13;
        packet.quad->u1 = u + 0x7F;
        packet.quad->v1 = i * 13;
        packet.quad->u2 = u;
        packet.quad->v2 = i * 13 + 12;
        packet.quad->u3 = u + 0x7F;
        packet.quad->v3 = i * 13 + 12;
        packet.quad->clut = func_80077AA4(0x3C0, 0x91);
        packet.quad->tpage = func_80077A64(0, 2, 0x3C0, 0);
        buffer = D_8019C9C0;
        address.pointer = packet.quad;
        packet.quad->tag.packed = (packet.quad->tag.packed & 0xFF000000)
                         | (buffer->allocation[9].packed & 0xFFFFFF);
        buffer->allocation[9].packed =
            (buffer->allocation[9].packed & 0xFF000000)
            | (address.word & 0xFFFFFF);
        packet.quad++;
        buffer->data = packet.bytes;
    }
    return value;
}
