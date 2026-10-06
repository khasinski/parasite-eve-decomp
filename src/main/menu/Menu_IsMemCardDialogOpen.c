#include "pe1/menu_inventory.h"

int Menu_IsMemCardDialogOpen(void) {
    return MenuWidget_FindByModeAndSelectedBase(1, 0x24) != 0;
}
