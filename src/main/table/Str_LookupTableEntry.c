typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

extern u8 g_GlyphMetricsTable[];
extern int g_StrLookupTableOffset;
extern int D_800A804C;

void *Str_LookupTableEntry(int arg0) {
    u8 *base;
    u8 *table;
    int index;
    s16 offset;

    index = g_GlyphMetricsTable[arg0 + D_800A804C];
    if (index == 0) {
        return 0;
    }

    base = g_GlyphMetricsTable + g_StrLookupTableOffset;
    table = base + *(int *)(base + 0x10);
    index += 0x7F;
    if ((u32)index >= *(u16 *)table) {
        return 0;
    }

    offset = *(s16 *)(table + 2 + index * 2);
    return table + offset;
}
