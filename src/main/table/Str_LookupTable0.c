typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern int g_StrLookupTableOffset;

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
