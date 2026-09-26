#include "pe1/boot_display.h"
s32 Boot_DrawTextBlock(void) {
    const u8 *line;
    u32 x = 24;
    u32 width;
    const u8 *scan;
    u32 rows = 0;
    s32 y = g_BootDisplayTextScrollX >> 8;
    g_BootDisplayTextCursor = D_80125C88;
    line = *g_BootDisplayTextCursor;
    while (*line != 255) {
        if (y >= -7) {
            if (y >= 225) break;
            while (*line) {
                switch (*line) {
                case ' ':
                    break;
                case '\\':
                    width = 0;
                    if (line[1]) {
                        scan = line + 1;
                        do {
                            width += Boot_GetGlyphWidth(*scan++);
                        } while (*scan);
                    }
                    x = (320 - width) >> 1;
                    goto next_character;
                case '\t':
                    x += Boot_GetGlyphWidth(' ') * 14;
                    goto next_character;
                default:
                    g_BootDisplayCallbacks->glyph((s16)x, (s16)y, *line);
                    break;
                }
                x += Boot_GetGlyphWidth(*line);
            next_character:
                ++line;
            }
            g_BootDisplayFontStyle = 0;
            x = 24;
            ++rows;
        }
        ++g_BootDisplayTextCursor;
        line = *g_BootDisplayTextCursor;
        y += 18;
    }
    g_BootDisplayTextScrollX += g_BootDisplayTextScrollStepX;
    return rows < 1;
}
