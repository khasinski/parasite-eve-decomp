/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned char *g_MenuItemIconGlyphTable;

void Draw_OffsetCursor(int x, int y);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_AllocTexturedQuad(int);

void Menu_DrawItemIcon(int arg0) {
    Draw_StatePush();
    Draw_OffsetCursor(2, 0);
    Draw_AllocTexturedQuad(g_MenuItemIconGlyphTable[arg0]);
    Draw_StatePop();
}
