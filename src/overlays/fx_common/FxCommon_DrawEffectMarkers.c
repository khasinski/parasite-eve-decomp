#include "fx_common_motion.h"

#define FX_MODE_PACKET                                          \
    (D_8019C9C0 == &g_FxCommonFrames[0].buffer ? &g_FxCommonDrawModes1[1] \
                                               : &g_FxCommonDrawModes1[0])

/* Fade the effect markers in or out by eight steps and draw, for every
 * enabled effect, a line from its projected position to a two-part label
 * (the selected effect uses the brighter palette). */
s16 func_80193B5C(s16 value)
{
    s16 level;
    FxCommonDrawModePacket *mode;
    FxCommonBuffer *buffer;
    FxCommonBuffer *leftBuffer;
    FxCommonBuffer *rightBuffer;
    FxCommonPacketTag *ot;
    FxCommonAddress address;
    FxCommonAddress lineLink;
    FxCommonAddress leftLink;
    FxCommonAddress rightLink;
    FxCommonLinePacket *line;
    FxCommonMarkerQuad *quad;
    GteShortVector rotation;
    GteShortVector position;
    GteShortVector unused;
    s32 screen0;
    s32 screen1;
    s32 screen2;
    s32 depth;
    s32 flag;
    u32 xy;
    u32 *vertex;
    u8 u;
    s16 x;
    s16 y;
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

    mode = FX_MODE_PACKET;
    mode->tag.bits.address = D_8019C9C0->allocation[10].bits.address;
    ot = &D_8019C9C0->allocation[10];
    address.pointer = FX_MODE_PACKET;
    ot->bits.address = address.word;

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

        line = (FxCommonLinePacket *)D_8019C9C0->data;
        line->tag.bytes.length = 3;
        line->code = 0x40;
        func_80077B04(line, 1);
        vertex = &line->xy0;
        *vertex++ = screen0;
        *vertex = screen1;
        if (D_8019CC52 == i) {
            line->r = level >> 1;
            line->g = level >> 1;
            line->b = level >> 1;
        } else {
            line->r = level >> 3;
            line->g = level >> 3;
            line->b = level >> 3;
        }
        buffer = D_8019C9C0;
        lineLink.pointer = line;
        line->tag.bits.address = buffer->allocation[10].bits.address;
        buffer->allocation[10].bits.address = lineLink.word;
        line++;
        buffer->data = (u8 *)line;
        quad = (FxCommonMarkerQuad *)buffer->data;

        xy = screen1;
        x = xy;
        y = xy >> 16;
        u = 0;
        func_80077BE4(quad);
        func_80077B04(quad, 1);
        quad->r0 = level >> 1;
        quad->g0 = level >> 1;
        quad->b0 = level >> 1;
        quad->r1 = level >> 1;
        quad->g1 = level >> 1;
        quad->b1 = level >> 1;
        quad->r2 = level >> 1;
        quad->g2 = level >> 1;
        quad->b2 = level >> 1;
        quad->r3 = level >> 1;
        quad->g3 = level >> 1;
        quad->b3 = level >> 1;
        quad->x0 = x;
        quad->y0 = y;
        quad->x1 = x + 0x7F;
        quad->y1 = y;
        quad->x2 = x;
        quad->y2 = y + 12;
        quad->x3 = x + 0x7F;
        quad->y3 = y + 13;
        quad->u0 = u;
        quad->v0 = i * 13;
        quad->u1 = u + 0x7F;
        quad->v1 = i * 13;
        quad->u2 = u;
        quad->v2 = i * 13 + 12;
        quad->u3 = u + 0x7F;
        quad->v3 = i * 13 + 12;
        if (D_8019CC52 == i)
            quad->clut = func_80077AA4(0x3C0, 0x90);
        else
            quad->clut = func_80077AA4(0x3C0, 0x8F);
        quad->tpage = func_80077A64(0, 1, 0x3C0, 0);
        leftBuffer = D_8019C9C0;
        leftLink.pointer = quad;
        quad->tag.bits.address = leftBuffer->allocation[9].bits.address;
        leftBuffer->allocation[9].bits.address = leftLink.word;
        quad++;

        u = 0x80;
        func_80077BE4(quad);
        func_80077B04(quad, 1);
        quad->r0 = u;
        quad->g0 = u;
        quad->b0 = u;
        quad->r1 = u;
        quad->g1 = u;
        quad->b1 = u;
        quad->r2 = u;
        quad->g2 = u;
        quad->b2 = u;
        quad->r3 = u;
        quad->g3 = u;
        quad->b3 = u;
        quad->x0 = x;
        quad->y0 = y;
        quad->x1 = x + 0x7F;
        quad->y1 = y;
        quad->x2 = x;
        quad->y2 = y + 12;
        quad->x3 = x + 0x7F;
        quad->y3 = y + 13;
        quad->u0 = u;
        quad->v0 = i * 13;
        quad->u1 = u + 0x7F;
        quad->v1 = i * 13;
        quad->u2 = u;
        quad->v2 = i * 13 + 12;
        quad->u3 = u + 0x7F;
        quad->v3 = i * 13 + 12;
        quad->clut = func_80077AA4(0x3C0, 0x91);
        quad->tpage = func_80077A64(0, 2, 0x3C0, 0);
        rightBuffer = D_8019C9C0;
        rightLink.pointer = quad;
        quad->tag.bits.address = rightBuffer->allocation[9].bits.address;
        rightBuffer->allocation[9].bits.address = rightLink.word;
        quad++;
        rightBuffer->data = (u8 *)quad;
    }
    return value;
}
