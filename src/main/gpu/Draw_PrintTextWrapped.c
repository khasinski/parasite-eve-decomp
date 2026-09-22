/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/menu_inventory.h"

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
