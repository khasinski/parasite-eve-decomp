/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_inventory.h"
#include "pe1/inventory.h"

void Menu_OnEquipConfirm(int unused, int confirmed)
{
    MenuWidgetNode *node;
    MenuWidgetNode *next;
    if (confirmed) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 7);
        if (D_8009CF18) {
            Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(node));
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : next);
            MenuWidget_NavScrollTo(0x35);
        } else if (!Inv_GetSlotItemData(MenuWidget_GridCellIndex(node))) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : next);
            MenuWidget_NavScrollTo(0x35);
        }
        Inv_BuildFilteredPackedList(D_8009CF18 ? 0x1FE : 0x200);
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node == MenuWidget_GetCurrentNode()) {
            node->cursor_y = 0;
            node->cursor_x = 0;
        }
        if (D_8009CF18) {
            MenuInput_SetPollingPaused(1);
            D_8009CFB0 = 1;
        }
    }
}
