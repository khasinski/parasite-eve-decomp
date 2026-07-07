/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;

void Menu_DrawSaveSlot2Item(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));
void Draw_SetTextDimmed(int value);
void Draw_AllocSprite(int arg0);
void Draw_OffsetCursor(int x, int y);

void Menu_DrawMemCardSlot2List(int arg0) {
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawSaveSlot2Item);
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
}
