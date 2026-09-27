#include "common.h"
#include "pe1/render_prim.h"
#include "pe1/draw_state.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void *Draw_LookupGlyphDescriptor(s32 index);
void BoundsCheck_AssertStub(s32 arg0);

extern u32 g_ActiveDrawBuffer;
extern s32 g_DrawPacketBufferBase;
extern s32 g_DrawTextDimmed;
extern s32 g_DrawPrimColor;
extern s32 g_DrawColorShaded;
extern s32 *g_OtListTail;
extern u16 g_TextCursorX;
extern u16 g_TextCursorY;

void Draw_EmitGlyph(s32 arg0, s32 arg1) {
    DrawGlyphDescriptor *glyph;
    u32 oldPacket;
    u32 nextPacket;
    RenderTexturedQuad *packet;
    u16 base_x;
    u16 base_y;
    s32 temp;
    u32 oldTag;
    s32 *ot;

    packet = 0;
    glyph = Draw_LookupGlyphDescriptor(arg0);
    /* Preserve the descriptor's register across packet setup. */
    asm volatile("" : "=r"(glyph) : "0"(glyph));
    oldPacket = g_ActiveDrawBuffer;
    nextPacket = oldPacket + 0x28;
    if (nextPacket < (u32)(g_DrawPacketBufferBase + 0x4000)) {
        g_ActiveDrawBuffer = nextPacket;
        packet = (RenderTexturedQuad *)oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }

    if (packet != 0) {
        if (g_DrawTextDimmed != 0) {
            packet->color.word = g_DrawColorShaded;
        } else {
            packet->color.word = g_DrawPrimColor;
        }
        packet->tag.bytes.length = 9;
        packet->color.bytes.code = 0x2C;
    }

    {
        register RenderTexturedQuad *ptr asm("$4");

        base_x = g_TextCursorX;
        base_y = g_TextCursorY;
        ptr = packet;
        *(volatile u16 *)&ptr->x2 = base_x;
        *(volatile u16 *)&ptr->x0 = base_x;
        *(volatile u16 *)&ptr->y1 = base_y;
        *(volatile u16 *)&ptr->y0 = base_y;

        {
            s32 sum;
            s32 glyphDim;

            glyphDim = *(volatile u8 *)&glyph->width;
            sum = *(volatile u16 *)&ptr->x0;
            temp = sum + glyphDim;
        }
        ptr->x3 = temp;
        ptr->x1 = temp;

        {
            s32 sum;
            s32 glyphDim;

            glyphDim = *(volatile u8 *)&glyph->height;
            sum = *(volatile u16 *)&ptr->y0;
            temp = sum + glyphDim;
        }
        ptr->y3 = temp;
        ptr->y2 = temp;

        if (arg1 == 2) {
            temp = glyph->u + glyph->width - 1;
            ptr->u2 = temp;
            ptr->u0 = temp;
            temp = glyph->v + glyph->height - 1;
            ptr->v1 = temp;
            ptr->v0 = temp;
            temp = glyph->u - 1;
            ptr->u3 = temp;
            ptr->u1 = temp;
            temp = glyph->v - 1;
            ptr->v3 = temp;
            ptr->v2 = temp;
        }
    }

    oldTag = *(volatile u32 *)&packet->tag.word;
    {
        u32 mask24;
        u32 maskTop;
        s32 clut;

        mask24 = 0xFFFFFF;
        clut = *(volatile u16 *)&glyph->clut;
        packet->clut = clut;
        ot = g_OtListTail;
        maskTop = 0xFF000000;
        packet->tpage = 7;
        packet->tag.word = (oldTag & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)packet & mask24);
    }
}
