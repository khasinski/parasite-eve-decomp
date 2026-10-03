#include "common.h"
#include "pe1/render_prim.h"
#include "pe1/text.h"
#include "pe1/draw_state.h"
/* CC1_FLAGS: -G8 -fno-cse-skip-blocks */
/* MASPSX_FLAGS: -G8 */

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern u32 *D_8009D11C;
extern int D_8009D124;
extern int D_8009D128;
extern int D_8009CDB0;
extern int D_8009D0D8;

int Draw_LookupGlyphMetrics(int glyph);
void Draw_AllocSprite(int glyph);
u16 GetTPage(int tp, int abr, int x, int y);
void BoundsCheck_AssertStub(int arg0, ...);

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
        y = *(volatile int *)&D_8009D128;
        D_8009D124 = x + 0xC;
        *(volatile int *)&D_8009D128 = y;
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
    if (next >= D_8009D104 + 0x4000) goto alloc_fail;
    D_8009D100 = next;
    prim = (RenderTexturedQuad *)old;
    goto alloc_done;
alloc_fail:
    BoundsCheck_AssertStub(1);
alloc_done:
    if (prim != 0) {
        if (D_8009D10C == 0) goto primary_color;
        prim->color.word = D_8009D114;
        goto color_done;
    primary_color:
        prim->color.word = D_8009D110;
    color_done:
        ((u8 *)prim)[3] = 9;
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
        register u32 tag asm("$3") = prim->tag.word;
        register u32 *ot asm("$7") = D_8009D11C;
        register int y asm("$5") = *(volatile int *)&D_8009D128;
        u32 otValue, linkedTag;
        register u32 otNew asm("$4");
        int newX;
        register int extra asm("$3");

        prim->tpage = tpage;
        otValue = *ot;
        *(volatile int *)&D_8009D128 = y;
        linkedTag = (tag & maskTop) | (otValue & mask24);
        asm volatile("" : : "r"(linkedTag));
        mask24 &= (u32)prim;
        prim->tag.word = linkedTag;
        asm volatile("" ::: "memory");
        extra = D_8009CDB0;
        otNew = *ot;
        extra = width + extra;
        newX = D_8009D124 + extra;
        asm volatile("" : "=r"(newX) : "0"(newX));
        otNew = (otNew & maskTop) | mask24;
        D_8009D124 = newX;
        *ot = otNew;
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
    goto test;
loop:
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
test:
    ch = *cursor;
    sentinel = 0xFF;
    terminator_check = ch & 0xFF;
    asm volatile("" : "=r"(terminator_check) : "0"(terminator_check));
    cursor++;
    if (terminator_check != sentinel) {
        goto loop;
    }
    cursor--;
    asm volatile("" : : "r"(cursor));
    return width;
}


#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
extern int g_TextCursorStackBottom[], g_TextCursorStackTop[];
typedef union TextCursorStackPointer {
    int *pointer;
    u32 word;
} TextCursorStackPointer;
extern TextCursorStackPointer g_TextCursorStackState __asm__("D_8009D12C");
#define g_TextCursorX D_8009D124
#define g_TextCursorY D_8009D128
#define g_TextCursorStackWord g_TextCursorStackState.word
#undef g_TextCursorStack
#define g_TextCursorStack g_TextCursorStackState.pointer

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
            M2C_FIELD(temp_a1, s32 *, 0) = t0;
            M2C_FIELD(temp_a1, s32 *, 4) = t1;
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
        if ((u32) &g_TextCursorStackBottom < (u32) g_TextCursorStackWord) {
            temp_v0 = M2C_FIELD(g_TextCursorStackWord, s32 *, -8);
            temp_a0 = M2C_FIELD(g_TextCursorStackWord, s32 *, -4);
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
        int *stack = g_TextCursorStack;
        if (stack < g_TextCursorStackTop) {
            int x = D_8009D124, y = D_8009D128;
            g_TextCursorStack = stack + 2;
            stack[0] = x;
            stack[1] = y;
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
        int *stack = g_TextCursorStack;
        if (stack < g_TextCursorStackTop) {
            stack[0] = D_8009D124;
            stack[1] = savedY;
            g_TextCursorStack = stack + 2;
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
            int *restore = g_TextCursorStack;
            if (g_TextCursorStackBottom < restore) {
                int x = restore[-2], y = restore[-1];
                g_TextCursorStack = restore - 2;
                D_8009D124 = x;
                D_8009D128 = y;
            } else BoundsCheck_AssertStub(3);
        }
    }
    {
        int *restore = g_TextCursorStack;
        if (g_TextCursorStackBottom < restore) {
            int x = restore[-2], y = restore[-1];
            g_TextCursorStack = restore - 2;
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
            M2C_FIELD(temp_a1, s32 *, 0) = t0;
            M2C_FIELD(temp_a1, s32 *, 4) = t1;
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
        if ((u32) &g_TextCursorStackBottom < (u32) g_TextCursorStackWord) {
            temp_v0 = M2C_FIELD(g_TextCursorStackWord, s32 *, -8);
            temp_a0 = M2C_FIELD(g_TextCursorStackWord, s32 *, -4);
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
        int *stack = g_TextCursorStack;
        if (stack < g_TextCursorStackTop) {
            int x = g_DrawSpriteX, y = g_DrawSpriteY;
            g_TextCursorStack = stack + 2;
            stack[0] = x;
            stack[1] = y;
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
            int *restore = g_TextCursorStack;
            if (g_TextCursorStackBottom < restore) {
                int x = restore[-2], y = restore[-1];
                g_TextCursorStack = restore - 2;
                g_DrawSpriteX = x;
                g_DrawSpriteY = y;
            } else BoundsCheck_AssertStub(3);
        }
        {
            int y = g_DrawSpriteY;
            /* Preserve the retail read/write of X during a vertical move. */
            int x = *(volatile int *)&g_DrawSpriteX;
            g_DrawSpriteX = x;
            g_DrawSpriteY = y + 14;
        }
    }
}
