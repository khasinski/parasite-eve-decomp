/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;
int g_MenuActionItemData;
int g_MenuSelectedItemData;

void Menu_DrawActionCodeItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawEquipItemDetailPanel(int arg0) {
    g_MenuActiveWidget = arg0;
    g_MenuActionItemData = g_MenuSelectedItemData;
    MenuWidget_DrawList(arg0, Menu_DrawActionCodeItem);
}
