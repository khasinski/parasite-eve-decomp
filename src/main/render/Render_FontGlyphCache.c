/* Font glyph cache: find the cache slot for a code, bind a code to a slot,
 * and look up or load a glyph. Contiguous default-profile functions. */
#include "common.h"
#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/font.h"

#define NULL ((void *)0)
M2C_UNK Render_LoadFontGlyph();
/* The cache-slot tail stores and reloads the selected byte through views of
 * its own symbol and the table pointer's, not through the record. */
extern struct { char _[16]; } D_80091A1F_o __asm__("g_FontGlyphIndex");
#define g_FontGlyphIndex (*(u8 *)&D_80091A1F_o)
extern u8 D_80091A1F_rd[] __asm__("g_FontGlyphIndex");
extern struct { char _[16]; } D_80091A28_o __asm__("g_FontGlyphTable");
#define g_FontGlyphTable (*(void **)&D_80091A28_o)

u8 Render_FindFontGlyphSlot(void) {
    u8 temp_v0;
    u8 temp_a0;
    u8 *hdr;
    u8 *p;
    u8 *p2;
    u8 *q;
    s32 i;
    s32 found;
    s32 slot;
    u8 *t;

    if (g_FontSelectionState.codeIndex >= 0x46U) {
        g_FontSelectionState.loadFailed = 1;
        return 0xFFU;
    }
    temp_v0 = g_FontSelectionState.codeIndex + 1;
    g_FontSelectionState.codeIndex = temp_v0;
    temp_a0 = D_8009EE22[temp_v0];
    g_FontSelectionState.loadFailed = 0;
    g_FontSelectionState.code = temp_a0;
    Render_LoadFontGlyph(temp_a0);
    found = 0;
    hdr = g_FontGlyphTable;
    p = hdr + 1;
    for (i = 0; i < hdr[3]; i++) {
        q = p + i;
        if (q[3] == 2) {
            found = i;
            i = p[2];
        }
    }
    p2 = p + 0x1B;
    for (i = 0; i < p[0x1B]; i++) {
        q = p2 + i;
        if (q[1] == (found & 0xFF)) {
            slot = i;
            goto store;
        }
    }
    slot = 0xFF;
store:
    g_FontGlyphIndex = slot;
    __asm__ volatile("" : : : "memory");
    t = g_FontGlyphTable;
    return *(t + *(t + D_80091A1F_rd[0] + 0x1D) + 4);
}

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

unsigned char Render_GetOrLoadFontGlyph(unsigned char action)
{
    unsigned char code, group;
    int i;
    FontGlyphSlots *slots;
    FontGlyphTable *table = g_FontSelectionState.table;
    code = table->groups.codes[table->slots.indices[g_FontSelectionState.selected]];
    if (code < 2 || code == 24 || code == 31 || code == 38 ||
        code == 45 || code == 52 || code == 59) {
        if (action == 20)
            return Render_FindFontGlyphSlot();
    }
    if (code == 2 && action == 22)
        return Render_StepFontLoad();
    slots = &g_FontSelectionState.table->slots;
    switch (action) {
    case 20:
        group = slots->groupKeys[g_FontSelectionState.selected];
        for (i = g_FontSelectionState.selected - 1; i >= 0; --i) {
            if (slots->groupKeys[i] == group) {
                g_FontSelectionState.selected = i;
                i = 0;
            }
        }
        break;
    case 21:
        --g_FontSelectionState.selected;
        break;
    case 22:
        group = slots->groupKeys[g_FontSelectionState.selected];
        for (i = g_FontSelectionState.selected + 1; i < slots->count; ++i) {
            if (slots->groupKeys[i] == group) {
                g_FontSelectionState.selected = i;
                i = slots->count;
            }
        }
        break;
    case 23:
        ++g_FontSelectionState.selected;
        break;
    }
    {
        FontGlyphTable *last = g_FontSelectionState.table;
        return last->groups.codes[last->slots.indices[g_FontSelectionState.selected]];
    }
}
