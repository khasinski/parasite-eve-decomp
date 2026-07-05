typedef unsigned char u8;

extern u8 *g_FontGlyphTable;

extern unsigned char D_80091A1C;

extern unsigned char g_FontGlyphLoadFailed;

extern int D_80091A24;

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

int Menu_IsEquipSlotActive(void) {
    return D_80091A1C != 0;
}

int Menu_ConsumeEquipSlotFlag(void) {
    unsigned char *ptr;

    ptr = &g_FontGlyphLoadFailed;
    if (*ptr != 0) {
        *ptr = 0;
        return 0;
    }

    return 0xFF;
}

int Menu_ConsumeEquipSlotInt(void) {
    int *ptr;

    ptr = &D_80091A24;
    if (*ptr != 0) {
        *ptr = 0;
        return 0;
    }

    return 0xFF;
}
