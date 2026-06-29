void Menu_DrawInventoryItemRow(void);
void Inv_SelectActiveList(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawWeaponList(int arg0) {
    Inv_SelectActiveList(0);
    MenuWidget_DrawList(arg0, Menu_DrawInventoryItemRow);
}
