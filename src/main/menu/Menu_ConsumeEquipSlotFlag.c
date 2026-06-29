extern unsigned char g_FontGlyphLoadFailed;

int Menu_ConsumeEquipSlotFlag(void) {
    unsigned char *ptr;

    ptr = &g_FontGlyphLoadFailed;
    if (*ptr != 0) {
        *ptr = 0;
        return 0;
    }

    return 0xFF;
}
