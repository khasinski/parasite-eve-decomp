#include "common.h"
#include "pe1/render_prim.h"
/* CC1_FLAGS: -G8 */
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
    register int raw asm("$3") = code;
    register int current asm("$4");
    int metrics;
    int width;
    register int nibble asm("$3");
    int u;
    int v;
    register int spacing asm("$5");
    int column;
    int row;
    register int arg0 asm("$4");
    RenderTexturedQuad *prim;
    u8 *old, *next;

    current = raw & 0xFF;
    if (D_8009D0D8 != 0) {
        asm volatile("" : "=r"(current) : "0"(current));
        current += D_8009D0D8 << 8;
        D_8009D0D8 = 0;
    }

    asm volatile("" : "=r"(raw) : "0"(raw));
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
    asm volatile("" : : "r"(nibble));
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
        asm volatile("" : : "r"(arg0));
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
        register u32 mask24 asm("$6") = 0xFFFFFF;
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


#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

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
    asm volatile("" : "=r"(ch) : "0"(ch));
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

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
extern s32 g_TextCursorX;
extern s32 g_TextCursorY;
extern u32 g_TextCursorStackPtr;
extern M2C_UNK g_TextCursorStackBottom[];
#define g_TextCursorStackBottom (g_TextCursorStackBottom[0])
extern M2C_UNK g_TextCursorStackTop[];
#define g_TextCursorStackTop (g_TextCursorStackTop[0])

void Draw_PrintRawText(u8 *arg0) {
    s32 temp_a0;
    s32 temp_v0;
    u32 temp_a1;
    u8 *var_s0;
    u8 var_a0;

    var_s0 = arg0;
    if (var_s0 != NULL) {
        temp_a1 = g_TextCursorStackPtr;
        if (temp_a1 < (u32) &g_TextCursorStackTop) {
            s32 t0 = g_TextCursorX; s32 t1 = g_TextCursorY;
            g_TextCursorStackPtr = temp_a1 + 8;
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
        if ((u32) &g_TextCursorStackBottom < (u32) g_TextCursorStackPtr) {
            temp_v0 = M2C_FIELD(g_TextCursorStackPtr, s32 *, -8);
            temp_a0 = M2C_FIELD(g_TextCursorStackPtr, s32 *, -4);
            g_TextCursorStackPtr -= 8;
            g_TextCursorX = temp_v0;
            g_TextCursorY = temp_a0;
            return;
        }
        BoundsCheck_AssertStub(3);
    }
}
