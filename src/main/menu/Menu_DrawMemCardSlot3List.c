/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;

void Menu_DrawSaveSlot3Item(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));
void Draw_SetTextDimmed(int arg0);
void Draw_AllocSprite(int arg0);

void Menu_DrawMemCardSlot3List(int arg0) {
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawSaveSlot3Item);
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
}
