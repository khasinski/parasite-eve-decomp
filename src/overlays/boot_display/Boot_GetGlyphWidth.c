#include "pe1/boot_display.h"

/* Four width choices per alphabetic glyph. The lowercase base accounts for
 * the extra 32 ASCII code points; odd styles select lowercase widths. */

u16 Boot_GetGlyphWidth(u8 character) {
    u16 style = g_BootDisplayFontStyle;
    u8 saved;
    s32 width;
    if (style == 2) {
        saved = character;
        if ((u8)(character - 'a') < 26)
            saved = character - 32;
        if ((u8)(saved - 'A') < 26)
            width = D_80125B1E[saved][0];
        else switch (saved) {
        case '!': case '\'': case '.': case ':': width = 3; break;
        case '#': width = 7; break;
        case '&': width = 11; break;
        case '(': case ')': case ',': case '-': case '/': width = 4; break;
        case '?': width = 6; break;
        case '@': width = 9; break;
        case '~': width = 8; break;
        default: width = 8; break;
        }
    } else {
        if ((u8)(character - 'A') < 26)
            width = D_80125B1C[character][style];
        else if ((u8)(character - 'a') < 26)
            width = g_BootDisplayLowercaseWidths[character][style | 1];
        else switch (character) {
        case '!': case '.': case ':': width = 2; break;
        case '\'': case ',': width = 3; break;
        case '(': case ')': case '-': case '/': case '?': width = 4; break;
        case '&': case '@': width = 9; break;
        case '~': width = 6; break;
        default: width = 6; break;
        }
    }
    return width;
}
