/* CC1_FLAGS: -G8 -fno-cse-skip-blocks */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"
#include "pe1/draw_level_bar.h"

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

#include "pe1/text.h"

extern int D_8009CDB0;
extern int D_8009D0D8;

u16 GetTPage(int tp, int abr, int x, int y);

void Draw_AllocTexturedQuad(int code) {
    int low;
    int glyph;
    int raw = code;
    int current;
    int metrics;
    int width;
    int nibble;
    int u;
    int v;
    int spacing;
    int column;
    int row;
    register int arg0 asm("$4");
    RenderTexturedQuad *prim;
    u8 *old, *next;

    current = raw & 0xFF;
    if (D_8009D0D8 != 0) {
        current += D_8009D0D8 << 8;
        D_8009D0D8 = 0;
    }

    low = raw & 0xFF;
    if ((u32)low >= 0xFA) {
        D_8009D0D8 = low - 0xFA;
        current = -1;
    }

    glyph = current;
    if (glyph == 0x100) {
        int x, y;
        Draw_AllocSprite(0x77);
        x = D_8009D124;
        y = D_8009D128;
        D_8009D124 = x + 0xC;
        D_8009D128 = y;
        return;
    }

    if (glyph < 0) {
        return;
    }

    if (glyph >= 0x101) {
        glyph -= 0x13;
    }

    glyph %= 0x1B9;
    row = glyph / 0x15;
    column = glyph - row * 0x15;

    prim = 0;
    old = D_8009D100;
    next = old + sizeof(RenderTexturedQuad);
    if (next < D_8009D104 + 0x4000) {
        D_8009D100 = next;
        prim = (RenderTexturedQuad *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (prim != 0) {
        if (D_8009D10C != 0) {
            prim->color.word = D_8009D114;
        } else {
            prim->color.word = D_8009D110;
        }
        prim->tag.bytes.length = 9;
        prim->color.bytes.code = 0x2C;
    }

    metrics = Draw_LookupGlyphMetrics(glyph);
    nibble = metrics & 0xF;
    width = (metrics >> 4) & 0xF;
    spacing = 0;
    if (glyph < 10 || glyph == 15) {
        spacing = 1;
    }
    u = column * 0xC + nibble;
    v = row * 0xC;
    asm volatile("" : : "r"(u), "r"(v));
    {
        int uBase;
        int vBase;
        register int half asm("$5");
        register int cursorX asm("$6");
        register int cursorY asm("$7");
        arg0 = 0;
        cursorX = D_8009D124;
        cursorY = D_8009D128;
        spacing++;
        D_8009CDB0 = spacing;

        prim->u2 = u;
        prim->u0 = u;
        prim->clut = 0x89C;
        asm volatile("" : "=r"(spacing) : "0"(spacing));
        half = spacing >> 1;
        prim->v1 = v;
        prim->v0 = v;
        /* Use the bytes stored in the packet after truncation. */
        uBase = *(volatile u8 *)&prim->u0;
        vBase = *(volatile u8 *)&prim->v0;
        uBase += width;
        vBase += 0xC;
        cursorX += half;
        cursorY++;
        asm volatile("" : "=r"(cursorX), "=r"(cursorY) : "0"(cursorX), "1"(cursorY));
        prim->u3 = uBase;
        prim->u1 = prim->u3;
        prim->x0 = cursorX;
        prim->x2 = prim->x0;
        prim->y1 = cursorY;
        prim->y0 = prim->y1;
        prim->v3 = vBase;
        prim->v2 = prim->v3;
        prim->x3 = prim->x0 + width;
        prim->x1 = prim->x3;
        prim->y3 = prim->y0 + 0xC;
        prim->y2 = prim->y3;
    }
    {
        u16 tpage = GetTPage(arg0, 0, 0x140, 0);
        u32 mask24 = 0xFFFFFF;
        register u32 maskTop asm("$8") = 0xFF000000;
        register int linkValue asm("$3") = prim->tag.word;
        register u32 *ot asm("$7") = D_8009D11C;
        register int y asm("$5") = D_8009D128;
        u32 otValue, linkedTag;
        int newX;

        prim->tpage = tpage;
        otValue = *ot;
        D_8009D128 = y;
        linkedTag = (linkValue & maskTop) | (otValue & mask24);
        asm volatile("" : : "r"(linkedTag));
        mask24 &= (u32)prim;
        prim->tag.word = linkedTag;
        asm volatile("" ::: "memory");
        linkValue = D_8009CDB0;
        arg0 = *ot;
        linkValue = width + linkValue;
        newX = D_8009D124 + linkValue;
        asm volatile("" : "=r"(newX) : "0"(newX));
        arg0 = (arg0 & maskTop) | mask24;
        D_8009D124 = newX;
        *ot = arg0;
    }
}



int Draw_MeasureTextWidth(u8 *text) {
    u8 *cursor;
    int width;
    int ch;
    int glyph;
    int terminator_check;
    register int sentinel asm("$2");
    int escape_page;
    int spacing;
    int metrics;

    cursor = text;
    width = 0;
    for (;;) {
        ch = *cursor;
        sentinel = 0xFF;
        terminator_check = ch & 0xFF;
        asm volatile("" : "=r"(terminator_check) : "0"(terminator_check));
        cursor++;
        if (terminator_check == sentinel) break;
        glyph = ch & 0xFF;
        if (D_8009D0D8 != 0) {
            glyph += D_8009D0D8 << 8;
            D_8009D0D8 = 0;
        }
        ch &= 0xFF;
        if ((unsigned int)ch >= 0xFA) {
            escape_page = ch - 0xFA;
            D_8009D0D8 = escape_page;
            glyph = -1;
        }
        ch = glyph;
        if (ch >= 0) {
            spacing = 0;
            if (ch < 10 || ch == 15) {
                spacing = 1;
            }
            D_8009CDB0 = spacing + 1;
            if (ch >= 0x100) {
                ch -= 0x13;
            }
            metrics = Draw_LookupGlyphMetrics(ch);
            width += ((metrics >> 4) & 0xF) + D_8009CDB0;
        }
    }
    cursor--;
    asm volatile("" : : "r"(cursor));
    return width;
}


#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
typedef union TextCursorStackPointer {
    int *pointer;
    u32 word;
} TextCursorStackPointer;
extern TextCursorStackPointer g_TextCursorStackState __asm__("D_8009D12C");
#define g_TextCursorX D_8009D124
#define g_TextCursorY D_8009D128
#define g_TextCursorStackWord g_TextCursorStackState.word

void Draw_PrintRawText(u8 *arg0) {
    s32 temp_a0;
    s32 temp_v0;
    u32 temp_a1;
    u8 *var_s0;
    u8 var_a0;

    var_s0 = arg0;
    if (var_s0 != NULL) {
        temp_a1 = g_TextCursorStackWord;
        if (temp_a1 < (u32) &g_TextCursorStackTop) {
            s32 t0 = g_TextCursorX; s32 t1 = g_TextCursorY;
            g_TextCursorStackWord = temp_a1 + 8;
            ((DrawTextCursorPair *)temp_a1)->x = t0;
            ((DrawTextCursorPair *)temp_a1)->y = t1;
        } else {
            BoundsCheck_AssertStub(2, temp_a1);
        }
        var_a0 = *var_s0;
        if ((var_a0 & 0xFF) != 0xFF) {
            do {
                var_s0 += 1;
                Draw_AllocTexturedQuad(var_a0);
                var_a0 = *var_s0;
            } while (var_a0 != 0xFF);
        }
        if ((u32) &g_TextCursorStackBottom < g_TextCursorStackWord) {
            temp_v0 = ((DrawTextCursorPair *)(g_TextCursorStackWord - 8))->x;
            temp_a0 = ((DrawTextCursorPair *)(g_TextCursorStackWord - 8))->y;
            g_TextCursorStackWord -= 8;
            g_TextCursorX = temp_v0;
            g_TextCursorY = temp_a0;
            return;
        }
        BoundsCheck_AssertStub(3);
    }
}

static inline int DecodeGlyph(int input) {
    register int code asm("$4") = input;
    int glyph;
    glyph = (u8)code;
    if (D_8009D0D8) {
        int page = D_8009D0D8 << 8;
        glyph += page;
        D_8009D0D8 = 0;
    }
    if ((u8)code >= 250) {
        D_8009D0D8 = (u8)code - 250;
        glyph = -1;
    }
    return glyph;
}

void Draw_PrintCenteredTextInWidth(u8 *text, int width) {
    u8 *cursor;
    int measured;
    int savedY;
    {
        int *stack = g_TextCursorStackState.pointer;
        if (stack < g_TextCursorStackTop) {
            int x = D_8009D124, y = D_8009D128;
            g_TextCursorStackState.pointer = stack + 2;
            ((DrawTextCursorPair *)stack)->x = x;
            ((DrawTextCursorPair *)stack)->y = y;
        } else BoundsCheck_AssertStub(2);
    }
    cursor = text;
    /* Keep the original text pointer distinct from the measuring cursor. */
    asm("" : "=r"(cursor) : "0"(cursor));
    measured = 0;
    if (*text != 255) {
        do {
            int narrow;
            int glyph;
            glyph = DecodeGlyph(*cursor++);
            if (glyph >= 0) {
                narrow = 0;
                if (glyph < 10 || glyph == 15) narrow = 1;
                D_8009CDB0 = narrow + 1;
                if (glyph >= 256) glyph -= 19;
                measured += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 15)
                    + D_8009CDB0;
            }
        } while (*cursor != 255);
        cursor = text;
    }
    {
        int padded = measured + 4;
        int x;
        x = D_8009D124 + ((width - padded) >> 1);
        savedY = D_8009D128;
        D_8009D124 = x;
        D_8009D128 = savedY;
    }
    if (cursor) {
        int *stack = g_TextCursorStackState.pointer;
        if (stack < g_TextCursorStackTop) {
            ((DrawTextCursorPair *)stack)->x = D_8009D124;
            ((DrawTextCursorPair *)stack)->y = savedY;
            g_TextCursorStackState.pointer = stack + 2;
        } else BoundsCheck_AssertStub(2);
        {
            u8 code = *cursor;
            if ((code & 255) != 255) {
                do {
                    ++cursor;
                    Draw_AllocTexturedQuad(code);
                    code = *cursor;
                } while (code != 255);
            }
        }
        {
            int *restore = g_TextCursorStackState.pointer;
            if (g_TextCursorStackBottom < restore) {
                int x = ((DrawTextCursorPair *)(restore - 2))->x, y = ((DrawTextCursorPair *)(restore - 2))->y;
                g_TextCursorStackState.pointer = restore - 2;
                D_8009D124 = x;
                D_8009D128 = y;
            } else BoundsCheck_AssertStub(3);
        }
    }
    {
        int *restore = g_TextCursorStackState.pointer;
        if (g_TextCursorStackBottom < restore) {
            int x = ((DrawTextCursorPair *)(restore - 2))->x, y = ((DrawTextCursorPair *)(restore - 2))->y;
            g_TextCursorStackState.pointer = restore - 2;
            D_8009D124 = x;
            D_8009D128 = y;
        } else BoundsCheck_AssertStub(3);
    }
}

extern int g_DrawTextBoxWidth;

void Draw_PrintCenteredText(u8 *text) {
    Draw_PrintCenteredTextInWidth(text, g_DrawTextBoxWidth);
}

void Draw_PrintTextById(unsigned int textId) {
    s32 temp_a0;
    s32 temp_v0;
    u32 temp_a1;
    u8 *var_s0;
    u8 var_a0;

    var_s0 = (u8 *)Str_LookupTable4(textId);
    if (var_s0 != NULL) {
        temp_a1 = g_TextCursorStackWord;
        if (temp_a1 < (u32) &g_TextCursorStackTop) {
            s32 t0 = g_TextCursorX; s32 t1 = g_TextCursorY;
            g_TextCursorStackWord = temp_a1 + 8;
            ((DrawTextCursorPair *)temp_a1)->x = t0;
            ((DrawTextCursorPair *)temp_a1)->y = t1;
        } else {
            BoundsCheck_AssertStub(2, temp_a1);
        }
        var_a0 = *var_s0;
        if ((var_a0 & 0xFF) != 0xFF) {
            do {
                var_s0 += 1;
                Draw_AllocTexturedQuad(var_a0);
                var_a0 = *var_s0;
            } while (var_a0 != 0xFF);
        }
        if ((u32) &g_TextCursorStackBottom < g_TextCursorStackWord) {
            temp_v0 = ((DrawTextCursorPair *)(g_TextCursorStackWord - 8))->x;
            temp_a0 = ((DrawTextCursorPair *)(g_TextCursorStackWord - 8))->y;
            g_TextCursorStackWord -= 8;
            g_TextCursorX = temp_v0;
            g_TextCursorY = temp_a0;
            return;
        }
        BoundsCheck_AssertStub(3);
    }
}


void Draw_PrintTextWrapped(u8 *text, int width) {
    while (*text != 0xFF) {
        int used;
        int *stack = g_TextCursorStackState.pointer;
        if (stack < g_TextCursorStackTop) {
            int x = g_DrawSpriteX, y = g_DrawSpriteY;
            g_TextCursorStackState.pointer = stack + 2;
            ((DrawTextCursorPair *)stack)->x = x;
            ((DrawTextCursorPair *)stack)->y = y;
        } else BoundsCheck_AssertStub(2);
        used = 0;
        while (used < width) {
            int code, glyph, narrow;
            if (*text == 0xFF) break;
            Draw_AllocTexturedQuad(*text);
            code = *text++;
            glyph = (u8)code;
            if (g_TextRenderMode) {
                glyph += g_TextRenderMode << 8;
                g_TextRenderMode = 0;
            }
            if ((u8)code >= 250) {
                g_TextRenderMode = (u8)code - 250;
                glyph = -1;
            }
            if (glyph >= 0) {
                narrow = 0;
                if (glyph < 10 || glyph == 15) narrow = 1;
                g_DrawGlyphAdvance = narrow + 1;
                if (glyph >= 256) glyph -= 19;
                used += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 15) + g_DrawGlyphAdvance;
            }
        }
        {
            int *restore = g_TextCursorStackState.pointer;
            if (g_TextCursorStackBottom < restore) {
                int x = ((DrawTextCursorPair *)(restore - 2))->x, y = ((DrawTextCursorPair *)(restore - 2))->y;
                g_TextCursorStackState.pointer = restore - 2;
                g_DrawSpriteX = x;
                g_DrawSpriteY = y;
            } else BoundsCheck_AssertStub(3);
        }
        {
            int y = g_DrawSpriteY;
            /* Preserve the retail read/write of X during a vertical move. */
            int x = g_DrawSpriteX;
            g_DrawSpriteX = x;
            g_DrawSpriteY = y + 14;
        }
    }
}


extern int D_8009D13C;
extern int D_8009D140;
extern int D_8009D144;

void func_8005F844(int variant) {
    D_8009D13C = variant != 0 ? 0x3A1C : 0x395D;
    D_8009D140 = variant != 0 ? 0xCC : 0x84;
    D_8009D144 = 0xA4;
}


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
        Draw_AllocSprite(sprite);
        width = 2;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    } else if (value > 0) {
        sprite = 0x89;
        Draw_AllocSprite(sprite);
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
        Draw_AllocSprite(sprite);
        width = 3;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    } else if (value > 0) {
        sprite = 0x89;
        Draw_AllocSprite(sprite);
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

void Draw_PrintTimeValue(int value, int flag) {
    int x;
    int y;

    if (value > 360000) {
        value = 359999;
    }
    D_8009D13C = flag != 0 ? 0x3A1C : 0x395D;
    D_8009D140 = flag != 0 ? 0xCC : 0x84;
    D_8009D144 = 0xA4;

    Draw_EmitDigitSprite(value / 36000);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value / 3600);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    if (flag) { Draw_AllocSprite(0x9B); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 3; }
    else      { Draw_AllocSprite(0x4F); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 5; }
    g_DrawSpriteX = x; g_DrawSpriteY = y;
    PE1_COMPILER_MEMORY_BARRIER();

    Draw_EmitDigitSprite((value / 600) % 6);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value / 60);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    if (flag) { Draw_AllocSprite(0x9B); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 3; }
    else      { Draw_AllocSprite(0x4F); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 5; }
    g_DrawSpriteX = x; g_DrawSpriteY = y;
    PE1_COMPILER_MEMORY_BARRIER();

    Draw_EmitDigitSprite((value / 10) % 6);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value);

    D_8009D13C = 0x395D;
    D_8009D140 = 0x84;
    D_8009D144 = 0xA4;
}

static inline void SetCursor(u32 x, u32 y)
{
    D_8009D124 = x;
    D_8009D128 = y;
}

static inline void PushCursor(void)
{
    int *stack = g_TextCursorStack;
    if (stack < g_TextCursorStackTop) {
        int x = D_8009D124, y = D_8009D128;
        g_TextCursorStack = stack + 2;
        ((DrawTextCursorPair *)stack)->x = x;
        ((DrawTextCursorPair *)stack)->y = y;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

static inline void PopCursor(void)
{
    int *stack = g_TextCursorStack;
    if (g_TextCursorStackBottom < stack) {
        g_TextCursorStack = stack - 2;
        SetCursor(stack[-2], stack[-1]);
    } else {
        BoundsCheck_AssertStub(3);
    }
}

static inline void PrintSign(u8 *text)
{
    if (text) {
        PushCursor();
        while (*text != 255) {
            Draw_AllocTexturedQuad(*text++);
        }
        PopCursor();
    }
}

static inline int Digit(int leading, int value)
{
    if (!leading || value != 0) {
        return (u8)(value % 10);
    }
    return 15;
}

/* Print a signed decimal field; glyph 15 is padding, table entry 0x70 the
 * minus sign. The sign occupies one place and restores its own text cursor.
 * Retail callers use bounded widths; powers of ten and negation must fit int.
 */
void Draw_AllocTexturedRect(int value, int width)
{
    int divisor = 1;
    int i;

    for (i = 1; i < width; i++) {
        divisor *= 10;
    }
    if (value < 0) {
        value = -value;
        divisor /= 10;
        width--;
        while (value < divisor) {
            Draw_AllocTexturedQuad(15);
            divisor /= 10;
            width--;
        }
        PrintSign(Str_LookupTable4(0x70));
        SetCursor(D_8009D124 + 9u, D_8009D128);
    }
    for (i = 0; i < width; i++) {
        Draw_AllocTexturedQuad(Digit(i < width - 1, value / divisor));
        divisor /= 10;
    }
}

void Draw_PrintNumberWidth6(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 6);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 6;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth5(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 5);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 5;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth4(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 4);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 4;
    D_8009D128 = y;
}

void func_800605C4(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 3);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 3;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth2(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 2);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 2;
    D_8009D128 = y;
}

/* Moves the cursor and saves the new position. */
static inline void PushCursorAt(int x, int y)
{
    int *stack = g_TextCursorStack;

    SetCursor(x, y);
    if (stack < g_TextCursorStackTop) {
        ((DrawTextCursorPair *)stack)->x = x;
        ((DrawTextCursorPair *)stack)->y = y;
        g_TextCursorStack = stack + 2;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

/* Allocates one bar quad from the packet arena and sets its colour and
 * command (0x2C: textured four-point polygon). */
static inline RenderTexturedQuad *AllocBarQuad(void)
{
    RenderTexturedQuad *prim = 0;
    u8 *old = D_8009D100;
    u8 *next = old + sizeof(RenderTexturedQuad);

    if (next < D_8009D104 + 0x4000) {
        D_8009D100 = next;
        prim = (RenderTexturedQuad *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (prim != 0) {
        if (D_8009D10C != 0)
            prim->color.word = D_8009D114;
        else
            prim->color.word = D_8009D110;
        prim->tag.bytes.length = 9;
        prim->color.bytes.code = 0x2C;
    }
    return prim;
}

/* One textured quad of the bar: glyph texels starting `u` columns into the
 * glyph, from `left` to the right edge computed from the stored left edge. */
#define DRAW_BAR_SEGMENT(left, right_from_x0, u_offset) \
    { \
        u32 *ot; \
        quad = AllocBarQuad(); \
        quad->x0 = quad->x2 = left; \
        quad->y0 = quad->y1 = D_8009D128 + 1; \
        quad->x1 = quad->x3 = right_from_x0; \
        quad->y2 = quad->y3 = quad->y0 + glyph->height; \
        quad->u0 = quad->u2 = glyph->u + (u_offset); \
        quad->v0 = quad->v1 = glyph->v; \
        quad->u1 = quad->u3 = quad->u0; \
        quad->v2 = quad->v3 = quad->v0 + glyph->height; \
        quad->clut = glyph->clut; \
        quad->tpage = 7; \
        quad->tag.link.address = *D_8009D11C; \
        ot = D_8009D11C; \
        link.quad = quad; \
        *ot = (*ot & 0xFF000000) | (link.word & 0xFFFFFF); \
    }

/* Draws a stat value followed by a 48-pixel level bar filled up to
 * 48 - width pixels, built from slices of glyph 0x48. */
void Draw_AllocTexturedRectAlt(int value, int width)
{
    DrawGlyphDescriptor *glyph = Draw_LookupGlyphDescriptor(0x48);
    RenderDrawModePacket *mode;
    RenderTexturedQuad *quad;
    int drawMode;
    DrawLevelBarLink link;
    u8 *old, *next;
    u32 *ot;

    PushCursor();
    PushCursorAt(D_8009D124, D_8009D128 + 2);
    SetCursor(D_8009D124 + 0x27, D_8009D128 - 2);
    Draw_PrintNumberWidth2Unk(value);
    D_8009D110 = 0x808080;
    D_8009D114 = 0x404040;
    PopCursor();
    Draw_AllocSprite(0x49);

    if (width < 0x2F)
        DRAW_BAR_SEGMENT(D_8009D124 + 1, quad->x0 + 1, 0);
    if (width < 0x2E)
        DRAW_BAR_SEGMENT(D_8009D124 + 2, (s16)quad->x0 + 0x2E - width, 1);
    if (width < 0x30)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x30 - width, quad->x0 + 1, 2);
    if (width > 0)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x31 - width, quad->x0 + 1, 3);
    if (width >= 3)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x32 - width, (s16)quad->x0 - 1 + width, 4);
    if (width >= 2)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x31, quad->x0 + 1, 5);
    PopCursor();

    drawMode = glyph->mode;
    mode = 0;
    old = D_8009D100;
    next = old + sizeof(RenderDrawModePacket);
    if (next < D_8009D104 + 0x4000) {
        D_8009D100 = next;
        mode = (RenderDrawModePacket *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (mode != 0)
        SetDrawTPage((char *)mode, 0, 0, ((drawMode & 3) << 7) | 7);
    ot = D_8009D11C;
    mode->tag = (mode->tag & 0xFF000000) | (*ot & 0xFFFFFF);
    link.mode = mode;
    *ot = (*ot & 0xFF000000) | (link.word & 0xFFFFFF);
}
