#include "pe1/font.h"

unsigned char Render_DrawTextDigit(FontGlyphTable *table, unsigned char mode)
{
    unsigned char selected;
    int code;
    int i, count;
    FontGlyphGroups *groups;
    FontGlyphSlots *slots;
    /* Matching debt: retail reserves 16 stack bytes without accessing them.
     * The original locals are unknown; this preserves that frame on GCC 2.7.2. */
    int stack_pad[2];

    if (!table) table = D_80091A28;
    switch (mode) {
    case 0: code = 3; break;
    case 1: code = (unsigned char)((unsigned)D_80091A1D % 10) != 0; break;
    case 2: code = 2; break;
    default: code = 4; break;
    }
    selected = 0;
    i = 0;
    count = table->groups.count;
    groups = &table->groups;
    if (count > 0) {
        int limit;
        code &= 255;
        limit = count;
        do {
            if (groups->codes[i] == code) {
                selected = i;
                i = groups->count;
            }
            ++i;
        } while (i < limit);
    }
    slots = (FontGlyphSlots *)(groups + 1);
    for (i = 0; i < slots->count; ++i) {
        if (slots->indices[i] == selected) return i;
    }
    return 255;
}
