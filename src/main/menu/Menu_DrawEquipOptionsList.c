
#include "include_asm.h"
void Menu_DrawWeaponModSlots(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawEquipOptionsList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawWeaponModSlots);
}
