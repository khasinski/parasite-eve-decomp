#include "pe1/menu_equipment.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/battle_cmd.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Menu_OpenEquipScreen(int mode)
{
    MenuWidgetNode *parent, *container, *list;
    int count, offset;

    container = MenuWidget_FindByModeAndSelectedBase(2, 50);
    if (!container) {
        parent = MenuWidget_CreateSimpleNode(50, 0, 0, 0);
        container = MenuWidget_CreateNode(50, parent, parent);
        parent->update = (void (*)())Menu_StepInventoryCategory;
        container->draw = Menu_DrawEquipListContainer;
        if (!mode)
            Draw_SetPrimCallback(container, 3);
        MenuWidget_SetCurrentNode(container);
    }

    parent = MenuWidget_CreateSimpleNode(51, container, 0, 0);
    container = MenuWidget_CreateNode(51, parent, parent);
    parent->update = (void (*)())Menu_InventoryInputHandler;
    parent->disabled = 1;
    container->draw = Menu_DrawWeaponList;
    container->itemAction = Inv_RebuildWithSlotLimit;
    container->cursor_x = -1;
    if (mode)
        D_8009CF0C = 2;
    else
        D_8009CF0C = 1;

    count = Inv_StepScrollDisplay2(mode);
    BattleCmd_SyncActiveAmmo();
    Menu_SetBattleEquipMode(0);
    Draw_SetPrimCallback(container,
        Inv_TransferItemAlt2(mode ? 0x3803FE : 0xF400));

    offset = container->popup_node == 0;
    parent = MenuWidget_CreateSimpleNode(52, container, 0, 0);
    list = MenuWidget_CreateNode(52, parent, parent);
    parent->update = (void (*)())Menu_InventoryInputHandler;
    if (offset)
        MenuWidget_OffsetPosition(parent, -6, 0);
    list->draw = Menu_DrawArmorList;
    list->itemAction = Inv_RebuildWithSlotLimit;
    list->refreshItems = Menu_RebuildSelectableMask;
    list->cursor_x = -1;
    D_8009CF94 = -1;
    D_8009CF8C = -1;
    Draw_SetPrimCallback(list, count);
    list->linkedPrevious = container;
    container->linkedNext = list;

    if (!MenuWidget_FindByModeAndSelectedBase(1, 19)) {
        parent = MenuWidget_CreateSimpleNode(19, 0, 0, 0);
        parent->draw = Menu_DrawContextHelpText;
    }
    D_8009CF00 = 0;
    D_8009CEFC = 0;
    Queue_Init();
}
