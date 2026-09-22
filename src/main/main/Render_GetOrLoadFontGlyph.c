#include "pe1/font.h"

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
