/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"

int g_DrawTextDimmed;

void Draw_SetTextDimmed(int value) {
    g_DrawTextDimmed = value;
}

void Draw_AllocSprite(int index) {
    RenderSpritePacket *sprite = 0;
    register DrawGlyphDescriptor *glyph asm("$18");
    u8 *old, *next;

    u32 *page;
    int mode;
    glyph = Draw_LookupGlyphDescriptor(index);
    /* Finish saving the descriptor before starting the allocation. */
    asm("" : : "r"(glyph));
    old = g_DrawPacketCursor;
    next = old + sizeof(RenderSpritePacket);
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        sprite = (RenderSpritePacket *)old;
    } else BoundsCheck_AssertStub(1);
    if (sprite) {
        if (g_DrawColorSelect) sprite->color.word = g_DrawAlternateColor;
        else sprite->color.word = g_DrawPrimaryColor;
        sprite->tag.bytes.length = 4;
        sprite->color.bytes.code = 0x64;
    }
    {
        register u32 oldTag asm("$5");
        register u32 maskTop asm("$6");
        register u32 mask24;
        u32 *ot;
        oldTag = sprite->tag.word;
        sprite->x = g_DrawSpriteX;
        sprite->y = g_DrawSpriteY;
        sprite->u = glyph->u;
        sprite->v = glyph->v;
        sprite->clut = glyph->clut;
        sprite->width = glyph->width;
        sprite->height = glyph->height;
        maskTop = 0xff000000;
        ot = g_DrawOrderingTableEntry;
        mask24 = 0xffffff;
        sprite->tag.word = (oldTag & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)sprite & mask24);
    }
    mode = glyph->mode;
    page = 0;
    old = g_DrawPacketCursor;
    next = old + 8;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        page = (u32 *)old;
    } else BoundsCheck_AssertStub(1);
    if (page) {
        register char *drawPage asm("$4") = (char *)page;
        register int pageBits = (mode & 3) << 7;
        /* Prepare page bits before the two zero-valued call arguments. */
        asm("" : : "r"(pageBits), "r"(drawPage));
        SetDrawTPage(drawPage, 0, 0, pageBits | 7);
    }
    {
        u32 mask24, maskTop;
        u32 *ot;
        mask24 = 0xffffff;
        maskTop = 0xff000000;
        ot = g_DrawOrderingTableEntry;
        *page = (*page & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)page & mask24);
    }
}

void *Draw_LookupGlyphDescriptor(s32 index);
void BoundsCheck_AssertStub(s32 arg0);

extern u32 g_ActiveDrawBuffer;
extern s32 g_DrawPacketBufferBase;
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
    if (nextPacket < (g_DrawPacketBufferBase + 0x4000)) {
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
        ptr->x2 = base_x;
        ptr->x0 = base_x;
        ptr->y1 = base_y;
        ptr->y0 = base_y;

        {
            s32 sum;
            s32 glyphDim;

            glyphDim = glyph->width;
            sum = *(volatile u16 *)&ptr->x0;
            temp = sum + glyphDim;
        }
        ptr->x3 = temp;
        ptr->x1 = temp;

        {
            s32 sum;
            s32 glyphDim;

            glyphDim = glyph->height;
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

int g_DrawGlyphAdvance;

void Draw_SetGlyphAdvance(int arg0) {
    g_DrawGlyphAdvance = arg0;
}
