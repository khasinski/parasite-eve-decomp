/* CC1_FLAGS: -G8 -fno-cse-skip-blocks */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/menu_inventory.h"

static inline int DecodeGlyph(int input) {
    register int code asm("$4") = input;
    int glyph;
    glyph = (u8)code;
    if (g_TextRenderMode) {
        int page = g_TextRenderMode << 8;
        glyph += page;
        g_TextRenderMode = 0;
    }
    if ((u8)code >= 250) {
        g_TextRenderMode = (u8)code - 250;
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
            int x = g_DrawSpriteX, y = g_DrawSpriteY;
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
                g_DrawGlyphAdvance = narrow + 1;
                if (glyph >= 256) glyph -= 19;
                measured += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 15)
                    + g_DrawGlyphAdvance;
            }
        } while (*cursor != 255);
        cursor = text;
    }
    {
        int padded = measured + 4;
        int x;
        x = g_DrawSpriteX + ((width - padded) >> 1);
        savedY = g_DrawSpriteY;
        g_DrawSpriteX = x;
        g_DrawSpriteY = savedY;
    }
    if (cursor) {
        int *stack = g_TextCursorStack;
        if (stack < g_TextCursorStackTop) {
            stack[0] = g_DrawSpriteX;
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
                g_DrawSpriteX = x;
                g_DrawSpriteY = y;
            } else BoundsCheck_AssertStub(3);
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
}
