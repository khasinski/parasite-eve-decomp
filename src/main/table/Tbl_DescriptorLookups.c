#include "pe1/text.h"
#include "pe1/menu_widget.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/aya.h"

extern MenuWidgetSimpleDescriptor D_80092478[];

extern MenuWidgetGridDescriptor D_80092888[];

extern int D_800A8030;

extern int g_StrLookupTableOffset;

MenuWidgetSimpleDescriptor *MenuWidget_LookupSimpleDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &D_80092478[index];
}

MenuWidgetGridDescriptor *MenuWidget_LookupGridDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &D_80092888[index];
}

DrawGlyphDescriptor *Draw_LookupGlyphDescriptor(int index) {
    register char *base = (char *)&D_800A8030;
    register int offset = *(int *)base;

    base -= 8;
    return (DrawGlyphDescriptor *)(offset + base + index * sizeof(DrawGlyphDescriptor));
}

void *Str_LookupTable0(unsigned int arg0) {
    u8 *end;
    u8 *base;
    TextOffsetTable *table;
    s16 offset;

    end = (u8 *)&g_StrLookupTableOffset;
    base = end - 4;
    table = (TextOffsetTable *)(base + *(int *)end);
    if (arg0 < table->count) {
        offset = table->offsets[arg0];
        return (u8 *)table + offset;
    }

    return 0;
}


extern int D_800A8034[];
extern int D_800A8038[];

ItemDataRecord *Item_LookupBaseData(unsigned int index)
{
    int *endPtr;
    int base;
    unsigned int offset;
    int end;

    endPtr = D_800A8038;
    end = endPtr[0];
    base = D_800A8034[0];

    if (index >= ((unsigned int)(end - base) >> 5)) {
        return 0;
    }

    offset = index << 5;
    return (ItemDataRecord *)((char *)D_800A8038 - 0x10 + base + offset);
}


extern int g_StatGrowthTable[];

void *Stat_GetGrowthTable(int arg0) {
    return (void *)(g_StatGrowthTable[0] + (int)((char *)g_StatGrowthTable - 0x10) + (arg0 << 9));
}
