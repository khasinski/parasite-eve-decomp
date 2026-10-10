#include "pe1/text.h"
extern u8 g_GlyphMetricsTable[];
extern int g_StrLookupTableOffset;

void *Str_LookupTable4(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[0]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTable8(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[1]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTableC(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[2]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTable10(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[3]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}
extern int D_800A804C;

void *Str_LookupTableEntry(int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    int index;
    s16 offset;

    index = g_GlyphMetricsTable[arg0 + D_800A804C];
    if (index == 0) {
        return 0;
    }

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[3]);
    index += 0x7F;
    if ((u32)index >= table->count) {
        return 0;
    }

    offset = table->offsets[index];
    return (u8 *)table + offset;
}
