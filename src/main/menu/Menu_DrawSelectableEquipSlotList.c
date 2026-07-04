/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;

void Menu_DrawSelectableEquipSlotRow(int arg0);
void Inv_SelectActiveList(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(int));

void Menu_DrawSelectableEquipSlotList(int arg0) {
    Inv_SelectActiveList(0);
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawSelectableEquipSlotRow);
}
