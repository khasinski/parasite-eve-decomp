#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"

void Inv_FetchSelectedInventoryItem(void) {
    int node;

    node = MenuWidget_FindByModeAndSelectedBase(2, 7);
    if (node == 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        }
    }

    Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
}
