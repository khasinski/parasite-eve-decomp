void Menu_DrawEquipInfoOptionRow(void) __asm__("func_80050308");
void MenuWidget_DrawList(int arg0, void (*callback)(void));

asm(".globl func_800474A8");
asm("func_800474A8 = Menu_DrawEquipInfoOptionList");

void Menu_DrawEquipInfoOptionList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawEquipInfoOptionRow);
}
