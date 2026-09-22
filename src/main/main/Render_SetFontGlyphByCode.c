#include "pe1/font.h"
static inline int findSlot(int useDecimalGroup)
{
    unsigned char code;
    int i, count;
    int found;
    FontGlyphGroups *groups;
    FontGlyphSlots *slots;
    FontGlyphTable *table;

    code = useDecimalGroup ? (unsigned char)((unsigned)D_80091A1D % 10) != 0 : 3;
    found = 0;
    table = D_80091A28;
    i = 0;
    count = table->groups.count;
    groups = &table->groups;
    for (; i < count; ++i) {
        if (groups->codes[i] == (unsigned char)code) {
            found = i;
            i = groups->count;
        }
    }
    slots = (FontGlyphSlots *)(groups + 1);
    for (i = 0; i < slots->count; ++i) {
        if (slots->indices[i] == (unsigned char)found)
            return i;
    }
    return 255;
}

unsigned char Render_SetFontGlyphByCode(unsigned char code)
{
    int selected;
    if ((int)code - 2 >= 69) {
        D_80091A20 = 1;
        return 255;
    }
    if (code < 2) {
        D_80091A1D = 1;
        return 255;
    }
    D_80091A1D = code;
    code = D_8009EE22[code];
    D_80091A1E = code;
    Render_LoadFontGlyph(code);
    if (D_80091A20 == 1)
        selected = findSlot(1);
    else
        selected = findSlot(0);
    D_80091A1F = selected;
    /* Keep retail's store and reload of the selected byte. */
    asm volatile("" : "=m"(D_80091A1F) : "0"(D_80091A1F));
    {
        FontGlyphTable *table = D_80091A28;
        int index = D_80091A1F;
        D_80091A20 = 0;
        return table->groups.codes[table->slots.indices[index]];
    }
}
