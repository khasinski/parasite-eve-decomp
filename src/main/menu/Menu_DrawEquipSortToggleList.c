void Menu_DrawEquipSortToggleRow(void) __asm__("func_80050280");
void MenuWidget_DrawList(int arg0, void (*callback)(void));

asm(".globl func_8004732C");
asm("func_8004732C = Menu_DrawEquipSortToggleList");

void Menu_DrawEquipSortToggleList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawEquipSortToggleRow);
}
