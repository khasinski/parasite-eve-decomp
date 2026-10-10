#include "pe1/text.h"
extern u8 g_GlyphMetricsTable[];
extern int g_StrLookupTableOffset;

void *Str_LookupTable4(unsigned int arg0) {
    u8 *base;
    TextOffsetTable *table;
    s16 offset;

    base = g_GlyphMetricsTable + g_StrLookupTableOffset;
    table = (TextOffsetTable *)(base + *(int *)(base + 4));
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTable8(unsigned int arg0) {
    u8 *base;
    TextOffsetTable *table;
    s16 offset;

    base = g_GlyphMetricsTable + g_StrLookupTableOffset;
    table = (TextOffsetTable *)(base + *(int *)(base + 8));
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTableC(unsigned int arg0) {
    u8 *base;
    TextOffsetTable *table;
    s16 offset;

    base = g_GlyphMetricsTable + g_StrLookupTableOffset;
    table = (TextOffsetTable *)(base + *(int *)(base + 0xC));
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTable10(unsigned int arg0) {
    u8 *base;
    TextOffsetTable *table;
    s16 offset;

    base = g_GlyphMetricsTable + g_StrLookupTableOffset;
    table = (TextOffsetTable *)(base + *(int *)(base + 0x10));
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}
extern int D_800A804C;

void *Str_LookupTableEntry(int arg0) {
    u8 *base;
    TextOffsetTable *table;
    int index;
    s16 offset;

    index = g_GlyphMetricsTable[arg0 + D_800A804C];
    if (index == 0) {
        return 0;
    }

    base = g_GlyphMetricsTable + g_StrLookupTableOffset;
    table = (TextOffsetTable *)(base + *(int *)(base + 0x10));
    index += 0x7F;
    if ((u32)index >= table->count) {
        return 0;
    }

    offset = table->offsets[index];
    return (u8 *)table + offset;
}
