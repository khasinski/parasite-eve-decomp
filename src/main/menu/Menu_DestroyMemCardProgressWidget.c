#include "pe1/menu_inventory.h"

void Menu_DestroyMemCardProgressWidget(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x27));
}
