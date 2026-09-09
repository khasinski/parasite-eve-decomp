#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009D0D8;
extern int D_8009CDB0;

int Draw_LookupGlyphMetrics(int glyph);

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
    asm volatile("andi %0,%1,0xFF" : "=r"(terminator_check) : "r"(ch));
    if (terminator_check != sentinel) {
        cursor++;
        goto loop;
    }
    asm volatile("addiu $16,$16,-1");
    return width;
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK BoundsCheck_AssertStub();
M2C_UNK Draw_AllocTexturedQuad();
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
