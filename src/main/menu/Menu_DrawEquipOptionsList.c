void Menu_DrawWeaponModSlots(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

asm(".globl func_800471BC");
asm("func_800471BC = Menu_DrawEquipOptionsList");

void Menu_DrawEquipOptionsList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawWeaponModSlots);
}
