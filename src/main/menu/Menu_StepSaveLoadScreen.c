#include "pe1/menu_equipment.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Menu_StepSaveLoadScreen(void)
{
    MenuWidgetNode *container;
    MenuWidgetNode *parent;
    /* Matching debt: retain the shared transfer callback in $s0. */
    register MenuWidgetItemAction transfer asm("$16");
    MenuWidgetNode *list;
    int slot;

    parent = MenuWidget_CreateSimpleNode(12, 0, 0, 0);
    container = MenuWidget_CreateNode(12, parent, parent);
    parent->update = (void (*)())Menu_SaveLoadNavigate;
    container->draw = Menu_DrawArmorListContainer;
    MenuWidget_SetCurrentNode(container);

    parent = MenuWidget_CreateSimpleNode(13, container, 0, 0);
    container = MenuWidget_CreateNode(13, parent, parent);
    parent->update = (void (*)())Menu_InventoryInputHandler;
    parent->disabled = 1;
    container->draw = Menu_DrawArmorSelectionList;
    transfer = Inv_TransferItem;
    container->itemAction = transfer;
    MenuWidget_ClearColumnLayout(container);
    container->layout_flags &= ~4;
    Draw_SetPrimCallback(container, Menu_GetBattleCount());

    parent = MenuWidget_CreateSimpleNode(14, container, 0, 0);
    list = MenuWidget_CreateNode(14, parent, parent);
    parent->update = (void (*)())Menu_InventoryInputHandler;
    list->draw = Menu_DrawSelectableEquipSlotList;
    list->refreshItems = Menu_RebuildSelectableMask;
    list->itemAction = transfer;
    list->cursor_x = -1;
    D_8009CF94 = -1;
    D_8009CF8C = -1;
    Draw_SetPrimCallback(list, Inv_GetAyaSlotLimit());
    list->linkedPrevious = container;
    container->linkedNext = list;

    slot = Inv_FindFirstEmptySlot(Menu_GetBattleCount());
    if (slot >= 0) {
        MenuWidget_ClampCursor(list, 0, slot + Menu_GetBattleCount() - 1);
    } else {
        slot = Inv_FindFirstEmptySlot(1);
        if (slot >= 0)
            MenuWidget_ClampCursor(list, 0, slot + 7);
    }
    list->cursor_x = -1;
    Menu_CreateContextHelpPanel();
    D_8009CF00 = 0;
}
