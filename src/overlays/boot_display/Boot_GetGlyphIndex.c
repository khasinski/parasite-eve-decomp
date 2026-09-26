#include "pe1/boot_display.h"
/* Fallthrough follows consecutive punctuation slots in the font atlas. */
#define PUNCTUATION_INDEX(base) { \
    index = -1; \
    switch (saved) { \
    case ':': ++index; \
    case '?': ++index; \
    case '/': ++index; \
    case '.': ++index; \
    case ',': ++index; \
    case '\'': ++index; \
    case '-': ++index; \
    case ')': ++index; \
    case '(': ++index; \
    case '&': ++index; \
    case '#': ++index; \
    case '@': ++index; \
    case '!': ++index; \
    case '~': ++index; \
    default: index += (base); \
    } \
}
s32 Boot_GetGlyphIndex(u8 character) {
    u16 style = g_BootDisplayFontStyle;
    u8 saved = character;
    s32 index;
    if (style) {
        if ((u8)(character - 'a') < 26) index = saved - 32;
        else if ((u8)(character - 'A') < 26) index = saved - 65;
        else if ((u8)(character - '0') < 10) index = saved + 58;
        else PUNCTUATION_INDEX(26);
    } else {
        if ((u8)(character - 'A') < 26) index = saved - 25;
        else if ((u8)(character - 'a') < 26) index = saved - 31;
        else if ((u8)(character - '0') < 10) index = saved + 58;
        else PUNCTUATION_INDEX(92);
    }
    return (u8)index;
}
