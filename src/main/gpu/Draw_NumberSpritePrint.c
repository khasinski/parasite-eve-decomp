/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
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
        u32 oldTag;
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
        oldTag = *(volatile u32 *)&p->tag.word;
        {
            u8 value = *(volatile u8 *)&p->v1;
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

void Draw_AllocSprite(int arg0);

void Draw_PrintNumberWidth2Unk(int value) {
    int width = 2;
    int place = 1;
    int i;
    int x;
    int y;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 1;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintNumberWidth3Unk(int value) {
    int width = 3;
    int place = 1;
    int i;
    int x;
    int y;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 2;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintSignedNumberWidth3(int value) {
    int width = 3;
    int place = 1;
    int i;
    int x;
    int y;
    int sprite;

    if (value < 0) {
        value = -value;
        sprite = 0x52;
    } else {
        if (value <= 0) {
            goto digits;
        }
        sprite = 0x89;
    }
    Draw_AllocSprite(sprite);
    width = 2;
    x = g_DrawSpriteX;
    y = g_DrawSpriteY;
    g_DrawSpriteX = x + 5;
    g_DrawSpriteY = y;
digits:
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintNumberWidth4Unk(int value) {
    int width = 4;
    int place = 1;
    int i;
    int x;
    int y;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 3;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintSignedNumberWidth4(int value) {
    int width = 4;
    int place = 1;
    int i;
    int x;
    int y;
    int sprite;

    if (value < 0) {
        value = -value;
        sprite = 0x52;
    } else {
        if (value <= 0) {
            goto digits;
        }
        sprite = 0x89;
    }
    Draw_AllocSprite(sprite);
    width = 3;
    x = g_DrawSpriteX;
    y = g_DrawSpriteY;
    g_DrawSpriteX = x + 5;
    g_DrawSpriteY = y;
digits:
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}
