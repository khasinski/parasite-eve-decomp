#include "common.h"
#include "pe1/menu_widget.h"

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

void *Draw_LookupGlyphDescriptor(int index) {
    register char *base = (char *)&D_800A8030;
    register int offset = *(int *)base;

    base -= 8;
    return (void *)(offset + base + index * 8);
}

void *Str_LookupTable0(unsigned int arg0) {
    u8 *end;
    u8 *base;
    u8 *table;
    s16 offset;

    end = (u8 *)&g_StrLookupTableOffset;
    base = end - 4;
    table = base + *(int *)end;
    if (arg0 < *(u16 *)table) {
        offset = *(s16 *)(table + 2 + arg0 * 2);
        return table + offset;
    }

    return 0;
}
