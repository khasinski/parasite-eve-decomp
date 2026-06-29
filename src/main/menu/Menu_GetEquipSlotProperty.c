typedef unsigned char u8;

extern u8 *g_FontGlyphTable;

int Menu_GetEquipSlotProperty(int arg0) {
    u8 *base;
    u8 *cursor;
    u8 index;

    base = g_FontGlyphTable;
    cursor = base + (u8)arg0;
    index = cursor[0x1D];
    base += index;
    return base[4];
}
