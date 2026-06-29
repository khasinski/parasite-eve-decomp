/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuEquipMode;

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_FlushPrimList(int arg0);
void Draw_SetColor(int arg0);
void Draw_PrintTextById(int arg0);

void Menu_DrawEquipScreenHeader(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_FlushPrimList(0x69);

    if (g_MenuEquipMode != 0) {
        Draw_SetColor(0x78703C);
        Draw_OffsetCursor(0xA, 0x16);
        Draw_PrintTextById(0x1F);
        Draw_OffsetCursor(0, 0x10);
        Draw_PrintTextById(0x21);
        Draw_SetColor(0x808080);
    }
}
