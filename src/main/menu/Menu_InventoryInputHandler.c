/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_inventory.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"

extern int D_8009CF00, D_8009CF5C, D_8009CF8C, D_8009CF90, D_8009CF94;
extern int D_8009CEFC;
void Inv_SelectActiveList(int);
int Inv_TestSelectionBit(int);
int Inv_CheckSlotUsable(int);
int Inv_GetWayneListItemByIndex(int);
int Inv_IsActiveListOverrideSelected(void);
int Inv_GetActiveListItemType(int);
int Inv_CanAddActiveListItemToAya(int);
void Menu_OpenItemActionSubmenu(MenuWidgetNode *, int, int);
int func_80057D18(int);
void func_800451D0(MenuWidgetNode *);

int Menu_InventoryInputHandler(MenuWidgetNode *root, unsigned int flags) {
    register MenuWidgetNode *child asm("$16");
    MenuWidgetNode *node;
    ItemDataRecord *item;
    int index;
    int active_list_flag;
    int submenu_flag;
    int selected_base;
    int result;
    register int action_result asm("$2");
    int value;
    unsigned int flags_saved = flags;
    register int handled asm("$19") = 0;
    int (*action)(int);

    /* Both the initial child index and the return value start at zero. */
    child = MenuWidget_GetChild(root, handled);
    active_list_flag = 0;
    if (root->selected_base == 0xD || root->selected_base == 0x34) active_list_flag = 1;
    Inv_SelectActiveList(active_list_flag);

    if (D_8009CF00 != 0) {
        if (flags_saved & 0x10000) {
            index = MenuWidget_GridCellIndex(child);
            item = Inv_LookupActiveListData(index);
            if (Inv_TestSelectionBit(index)) {
                if (item != 0) {
                    action = Inv_RemoveActiveListItem;
                    if (item->kind < 10) action = func_80057D18;
                    action_result = action(index);
                } else {
                    action_result = 0;
                }
                result = action_result;
                Inv_SetActiveList(0, &result);
                if (D_8009CF5C) Inv_CheckSlotUsable(D_8009CF5C);
                D_8009CF00 = 0;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
            return handled;
        }
        if (flags_saved & 0x40) {
            Inv_SetActiveList(9, 0);
            Menu_PlayCancelSound();
        }
        return handled;
    }

    if (flags_saved & 0x10000) {
        if (root->selected_base == 0x33) {
            index = Inv_GetWayneListItemByIndex(MenuWidget_GridCellIndex(child));
        } else {
            index = MenuWidget_GridCellIndex(child);
        }

        if (D_8009CF8C >= 0) {
            D_8009CF90 = Inv_IsActiveListOverrideSelected();
            D_8009CF94 = index;
            if (Inv_TestSelectionBit(index)) func_800451D0(child);
        } else if (MenuWidget_GridCellIndex(child) >= 0 &&
                   Inv_LookupActiveListData(index) != 0 && D_8009CEFC == 0) {
            if (child->target_x >= 0) child->target_x = -1;
            submenu_flag = root->selected_base == 0xD || root->selected_base == 0x34;
            Menu_OpenItemActionSubmenu(child, submenu_flag, index);
        } else {
            value = Inv_GetActiveListItemType(index);
            if (value >= 0x10 && Inv_GetActiveListItemType(index) < 0x13) {
                if (Inv_CanAddActiveListItemToAya(index)) {
                    handled = 1;
                    Menu_PlayErrorSound();
                    asm volatile("" : "=r"(handled) : "0"(handled));
                    return handled;
                }
            } else {
                child->target_x = child->cursor_x;
                child->target_y = child->cursor_y;
            }
        }
        handled = 1;
        Menu_PlayConfirmSound();
        asm volatile("" : "=r"(handled) : "0"(handled));
        return handled;
    }

    if (flags_saved & 0x40) {
        if (D_8009CF8C >= 0) {
            Inv_RebuildSelectableMask();
            D_8009CF8C = -1;
        } else if (child->target_x < 0) {
            selected_base = root->selected_base;
            if (selected_base == 0xD || selected_base == 0xE) {
                child->cursor_x = -1;
                node = MenuWidget_FindByModeAndSelectedBase(2, 0xC);
                MenuWidget_SetCurrentNode(node);
            } else if (selected_base == 1) {
                MenuWidget_DestroyNode(root);
                MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
                Menu_CreateBonusPointAllocationView();
            } else if (selected_base == 0x33 || selected_base == 0x34) {
                child->cursor_x = -1;
                child = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
                child->cursor_x = 0;
                MenuWidget_SetCurrentNode(child);
            }
        }
        Menu_PlayCancelSound();
        handled = 1;
        asm volatile("" : "=r"(handled) : "0"(handled));
        return handled;
    }

    return handled;
}
