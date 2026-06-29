/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;

int MenuWidget_GridCellIndex(int);
void Draw_OffsetCursor(int, int);
void Draw_SetTextDimmed(int);
void Draw_AllocSprite(int);

void Menu_DrawSaveSlot3Item(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(arg0 + 0x62);
}
