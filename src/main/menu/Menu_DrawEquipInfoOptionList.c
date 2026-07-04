void Menu_DrawEquipInfoOptionRow(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawEquipInfoOptionList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawEquipInfoOptionRow);
}
