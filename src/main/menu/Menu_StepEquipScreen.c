/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"

int Menu_StepEquipScreen(MenuWidgetNode *node, unsigned int flags)
{
    int handled = 0;
    Inv_SelectActiveList(D_8009CF10);
    if (flags & 0x10000) {
        switch (D_80092234[D_8009CDA8][
            MenuWidget_GridCellIndex(MenuWidget_GetChild(node, 0))]) {
        case 0: {
            int allowed = 0;
            int index = D_8009CF04;
            ItemDataRecord *item = Inv_LookupActiveListData(index);
            /* Kind 10 uses a byte at +0x0E, not the equipment signed bonus. */
            if (Inv_TestSelectionBit(index)) {
                if (D_8009CF0C != 1 || item->kind != 10 ||
                    *(u8 *)&item->bonusStats[0] < 4)
                    allowed = 1;
            }
            if (allowed) {
                MenuWidget_DestroyNode(node);
                if (D_8009CF0C == 2) {
                    MenuWidget_NavScrollTo(51);
                    MenuWidget_NavScrollTo(52);
                }
                if (Menu_GetBattleEquipMode())
                    MenuWidget_DestroyNode(MenuWidget_GetCurrentNode()->parent);
                Inv_BuildEquipSlotDisplay(D_8009CF04);
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        case 1: {
            MenuWidgetNode *parent = node->parent;
            parent->target_x = parent->cursor_x;
            parent->target_y = parent->cursor_y;
            if (!D_8009CF0C) {
                Inv_ClearSelectionBitset();
                Inv_SetSelectionBit(parent->x_limit * parent->cursor_y + parent->cursor_x);
            }
            MenuWidget_DestroyNode(node);
            Menu_PlayConfirmSound();
            break;
        }
        case 2:
            if (Inv_IsSlotSelectable(D_8009CF04)) {
                if (Inv_IsActiveListOverrideSelected() || D_8009CF04 != D_800C0E20.tracked[2] ||
                    Inv_GetActiveSlotCount(0))
                    Menu_StepItemDetailPanel();
                else
                    Menu_CreateNotificationDialog(29, 0);
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        case 3:
            if (!D_8009CF08) {
                MenuWidget_DestroyNode(node);
                Menu_OnInventoryItemConfirm(D_8009CF04);
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        handled = 1;
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
        handled = 1;
    }
    return handled;
}
