typedef unsigned char u8;

extern u8 g_FontGlyphIndex;
extern u8 D_80091A1C;
extern u8 g_FontGlyphCode;
extern int g_FontGlyphTable;

int Menu_ResetEquipSlotState(void) {
    g_FontGlyphIndex = 0xFF;
    D_80091A1C = 0;
    g_FontGlyphCode = 0;
    g_FontGlyphTable = 0;
    return 0;
}
