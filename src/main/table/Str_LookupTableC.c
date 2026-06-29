typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern u8 g_GlyphMetricsTable[];
extern int g_StrLookupTableOffset;

void *Str_LookupTableC(unsigned int arg0) {
    u8 *base;
    u8 *table;
    s16 offset;

    base = g_GlyphMetricsTable + g_StrLookupTableOffset;
    table = base + *(int *)(base + 0xC);
    if (arg0 >= *(u16 *)table) {
        return 0;
    }

    offset = *(s16 *)(table + 2 + arg0 * 2);
    return table + offset;
}
