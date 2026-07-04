void Menu_DrawEquipSortToggleRow(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawEquipSortToggleList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawEquipSortToggleRow);
}
