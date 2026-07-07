/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;

void Menu_DrawSaveSlot1Item(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));
void MenuWidget_GetCurrentNode(void);
void Draw_SetTextDimmed(int arg0);
void Draw_AllocSprite(int arg0);
void Draw_OffsetCursor(int arg0, int arg1);

void Menu_DrawMemCardSlot1List(int arg0) {
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawSaveSlot1Item);
    MenuWidget_GetCurrentNode();
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
}
