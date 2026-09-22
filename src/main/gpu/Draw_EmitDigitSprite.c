/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"

void Draw_EmitDigitSprite(int digit) {
    RenderTexturedQuad *quad = 0;
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + 40;

    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        quad = (RenderTexturedQuad *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (quad) {
        if (g_DrawColorSelect) quad->color.word = g_DrawAlternateColor;
        else quad->color.word = g_DrawPrimaryColor;
        quad->tag.bytes.length = 9;
        quad->color.bytes.code = 0x2C;
    }
    {
        RenderTexturedQuad *p;
        register u32 oldTag asm("$4");
        u32 mask24;
        u16 x = *(u16 *)&g_DrawSpriteX, y = *(u16 *)&g_DrawSpriteY;
        p = quad;
        p->x0 = p->x2 = x;
        p->y0 = p->y1 = y;
        x = p->x0 + 5;
        /* Narrow volatile views preserve the retail packet-field reloads. */
        y = *(volatile u16 *)&p->y0 + 7;
        p->x1 = p->x3 = x;
        p->y2 = p->y3 = y;
        if (digit >= 0) p->u2 = g_DrawDigitFontBaseTexU + (digit % 10) * 5;
        else p->u2 = 88;
        {
            u8 value = p->u2;
            p->u2 = value;
            p->u0 = value;
        }
        if (digit >= 0) p->v1 = g_DrawDigitFontBaseTexV;
        else p->v1 = 164;
        mask24 = 0xFFFFFF;
        oldTag = p->tag.word;
        {
            u8 value = p->v1;
            int rightU = *(volatile u8 *)&p->u0;
            int bottomV;
            p->v0 = value;
            bottomV = *(volatile u8 *)&p->v0;
            rightU += 5;
            p->v1 = value;
            p->u1 = p->u3 = rightU;
            p->v2 = p->v3 = bottomV + 7;
        }
        p->clut = g_DrawDigitFontTpageClut;
        p->tpage = 7;
        {
            u32 maskTop = 0xFF000000;
            u32 *ot = g_DrawOrderingTableEntry;
            p->tag.word = (oldTag & maskTop) | (*ot & mask24);
            mask24 &= (u32)p;
            *ot = (*ot & maskTop) | mask24;
        }
    }
}
