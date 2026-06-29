void Menu_DrawArmorItemRow(void);
void Inv_SelectActiveList(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawArmorList(int arg0) {
    Inv_SelectActiveList(1);
    MenuWidget_DrawList(arg0, Menu_DrawArmorItemRow);
}
