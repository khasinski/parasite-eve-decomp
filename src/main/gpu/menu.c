extern char D_80092478[];

extern char D_80092888[];

extern int D_800A8030;

typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern int g_StrLookupTableOffset;

void *MenuWidget_LookupSimpleDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return D_80092478 + index * 0x10;
}

void *MenuWidget_LookupGridDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return D_80092888 + index * 0x20;
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
