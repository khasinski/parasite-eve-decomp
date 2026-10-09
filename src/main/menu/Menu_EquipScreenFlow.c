/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/menu_context_help.h"
#include "pe1/text.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_widget.h"
#include "../../../tools/m2c/m2c_macros.h"

/* Inventory/equip list screen: creation, list input, the item-count header,
 * the per-item action submenu and its input step. Contiguous at 0x80044274
 * and joined by private data and helper calls. */

#define NULL ((void *)0)

void Inv_SwapSlots();
void Menu_DrawAmmoTypeHeader(MenuWidgetNode *node);
void Menu_CreateAmmoSpendPanel();
s32 func_80052F0C();
void Menu_DrawContextActionList();
int Battle_IsInputAllowedWrapped(void);
int Battle_GetStateFlag1(void);
int func_80057D18(int);
void func_800451D0(MenuWidgetNode *);
void Menu_OpenItemActionSubmenu(MenuWidgetNode *, int, int);
void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
void Draw_OffsetCursor(int x, int y);
int Inv_CountTotal(void);
void Draw_PrintNumberWidth2Unk(int value);
void Draw_AllocSprite(int sprite);

extern s32 g_MenuActionSubmenuOpen;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuEquipSwapSource;
extern s32 g_InvAmmoSpendActiveList;
extern s32 g_InvSwapSourceList;
extern int D_8009CF5C, D_8009CF90;
extern s32 g_MenuItemActionContext;
extern s32 g_MenuItemActionDisabled;
extern s32 g_MenuEquipMode;

void Menu_CreateEquipScreen(s32 arg0) {
    s32 entryIndex;
    MenuWidgetNode *currentNode;
    MenuWidgetNode *node;
    MenuWidgetNode *parent;

    MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0)->draw = Menu_DrawAmmoTypeHeader;
    parent = MenuWidget_CreateSimpleNode(1, arg0, 0, 0);
    node = MenuWidget_CreateNode(1, parent, parent);
    currentNode = node;
    parent->update = (void (*)())Menu_InventoryInputHandler;
    node->draw = Menu_DrawSelectableEquipSlotList;
    MenuWidget_SetCurrentNode(currentNode);
    node->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
    node->refreshItems = Menu_RebuildSelectableMask;
    Inv_RebuildSelectableMask();
    g_InvSwapTargetIndex = -1;
    g_InvSelectedItemIndex = -1;
    Draw_SetPrimCallback(node, Inv_GetAyaSlotLimit());
    g_MenuActionSubmenuOpen = 0;
    if (g_MenuEquipSwapSource != 0) {
        entryIndex = g_MenuEquipSwapSource - 1;
        node->cursor_x = (entryIndex & 1);
        node->cursor_y = ((entryIndex >> 1) & 0x7F);
        node->scroll_y = (entryIndex >> 8);
    }
}

void Menu_OnInventoryItemConfirm(s32 arg0) {
    s32 temp_v0;
    s32 var_s0;
    s32 var_s0_2;
    MenuWidgetNode *temp_s0;
    MenuWidgetNode *temp_s0_2;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_s1_2;

    g_InvAmmoSpendActiveList = func_80052F0C();
    g_InvSelectedItemIndex = arg0;
    temp_v0 = Inv_BuildCompatibleWeaponBitset(arg0);
    if (temp_v0 == 0) {
        g_InvSelectedItemIndex = -1;
        Inv_RebuildSelectableMask();
        return;
    }
    if (temp_v0 == 1) {
        temp_s0 = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
        temp_s1 = MenuWidget_FindByModeAndSelectedBase(2, 0xE);
        if ((temp_s0 != NULL) && (MenuWidget_GridCellIndex(temp_s0) != 0) && (temp_s1 != NULL)) {
            temp_s0->cursor_x = -1;
            Inv_SelectActiveList(0);
            var_s0 = 0;
            while ((var_s0 < Inv_GetAyaSlotLimit()) && (Inv_TestSelectionBit(var_s0) == 0)) {
                var_s0 += 1;
            }
            temp_s1->cursor_x = 0;
            temp_s1->cursor_y = (var_s0 & -(var_s0 < Inv_GetAyaSlotLimit()));
            MenuWidget_SetCurrentNode(temp_s1);
        }
        {
            u8 *cf90_addr;
            u8 *cf94_addr;
            s32 cf88_value;

            cf90_addr = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(cf90_addr) : "0"(cf90_addr));
            cf90_addr -= 0x3070;
            cf88_value = g_InvAmmoSpendActiveList;
            __asm__ volatile("" : "=r"(cf88_value) : "0"(cf88_value));
            cf94_addr = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(cf94_addr) : "0"(cf94_addr));
            cf94_addr -= 0x306C;
            g_InvSwapSourceList = cf88_value;
            __asm__ volatile("" : : "r"(cf90_addr), "r"(cf94_addr));
            Inv_InitSlotDisplay(cf90_addr, cf94_addr);
        }
        Menu_CreateAmmoSpendPanel(MenuWidget_GetCurrentNode());
        return;
    }
    temp_s0_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    temp_s1_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xE);
    if ((temp_s0_2 != NULL) && (MenuWidget_GridCellIndex(temp_s0_2) != 0) && (temp_s1_2 != NULL)) {
        temp_s0_2->cursor_x = -1;
        Inv_SelectActiveList(0);
        var_s0_2 = 0;
        while ((var_s0_2 < Inv_GetAyaSlotLimit()) && (Inv_TestSelectionBit(var_s0_2) == 0)) {
            var_s0_2 += 1;
        }
        temp_s1_2->cursor_x = 0;
        temp_s1_2->cursor_y = (var_s0_2 & -(var_s0_2 < Inv_GetAyaSlotLimit()));
        MenuWidget_SetCurrentNode(temp_s1_2);
    }
}

int Menu_InventoryInputHandler(MenuWidgetNode *root, unsigned int flags) {
    MenuWidgetNode *child;
    MenuWidgetNode *node;
    ItemDataRecord *item;
    int index;
    int active_list_flag;
    int submenu_flag;
    int selected_base;
    int result;
    int action_result;
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
                if (D_8009CF5C) Inv_AddItem(D_8009CF5C);
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
            if (child->appearance.cursorTargetX >= 0) child->appearance.cursorTargetX = -1;
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
                child->appearance.cursorTargetX = child->cursor_x;
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
        } else if (child->appearance.cursorTargetX < 0) {
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

void Menu_DrawAmmoTypeHeader(MenuWidgetNode *node) {
    MenuWidgetNode *list;
    MenuWidgetSimpleDescriptor *desc;
    int page_delta;
    int target_y;

    list = MenuWidget_FindByModeAndSelectedBase(2, 1);
    desc = MenuWidget_LookupSimpleDescriptor(1);
    target_y = (list->visible_rows << 4) + 4;
    MenuWidget_OffsetPosition(node, 0, (desc->y + target_y) - node->y);

    if (list->has_scroll != 0) {
        page_delta = (list->y_limit - list->visible_rows) - list->scroll_y;
        switch (page_delta) {
        case 0:
            MenuWidget_OffsetPosition(node, 0, list->scroll_adjust - 0x10);
            break;
        case 1:
            if (list->scroll_adjust < 0) {
                MenuWidget_OffsetPosition(node, 0, list->scroll_adjust);
            }
            break;
        }
    }

    Draw_OffsetCursor(0xA, 2);
    Draw_PrintRawText(Str_LookupTable4(0x25));
    Draw_OffsetCursor(0x46, 4);
    Draw_PrintNumberWidth2Unk(Inv_CountTotal());
    Draw_AllocSprite(0x4C);
    Draw_OffsetCursor(5, 0);
    Draw_PrintNumberWidth2Unk(Inv_GetAyaSlotLimit());
}

void Menu_OpenItemActionSubmenu(MenuWidgetNode *arg0, s32 arg1, s32 arg2) {
    s32 var_s0;
    s32 temp_s0;
    s32 temp_v0;
    s32 var_s0_2;
    s32 var_s2;
    MenuWidgetNode *temp_a0;
    ItemDataRecord *temp_s0_2;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_v0_2;

    g_MenuActiveItemList = arg1;
    Inv_SelectActiveList(arg1);
    g_MenuActiveItemSlot = arg2;
    temp_v0 = Inv_GetActiveListItemType(arg2);
    if (temp_v0 != 0xA) {
        if (((temp_v0 - 0xC) < 2U) || ((temp_v0 - 0xE) < 2U) || ((temp_v0 - 8) < 2U)) {
            g_MenuItemActionContext = 2;
        } else {
            g_MenuItemActionContext = 1;
        }
    } else {
        g_MenuItemActionContext = 0;
    }
    var_s0 = 2;
    if (g_MenuItemActionContext == var_s0) {
        var_s0 = 3;
    }
    temp_v0_2 = MenuWidget_CreateSimpleNode(var_s0, arg0, 0, 1);
    temp_s1 = MenuWidget_CreateNode(var_s0, temp_v0_2, temp_v0_2);
    temp_a0 = temp_s1;
    temp_v0_2->update = (void (*)())Menu_StepEquipScreen;
    temp_s1->draw = Menu_DrawContextActionList;
    MenuWidget_SetCurrentNode(temp_a0);
    var_s0_2 = 0;
    if ((g_MenuItemActionContext == 1) && ((Battle_IsInputAllowedWrapped() == 0) || (Inv_BuildCompatibleWeaponBitset(g_MenuActiveItemSlot) == 0) || (Battle_GetStateFlag1() != 0))) {
        var_s0_2 = 1;
    }
    g_MenuItemActionDisabled = var_s0_2;
    Inv_RebuildSelectableMask();
    if (g_MenuItemActionContext == 0) {
        var_s2 = 0;
        temp_s0 = g_MenuActiveItemSlot;
        temp_s0_2 = Inv_LookupActiveListData(temp_s0);
        if ((Inv_TestSelectionBit(temp_s0) != 0) && ((g_MenuEquipMode != 1) || (temp_s0_2->kind != 0xA) || ((u8) M2C_FIELD(temp_s0_2, u8 *, 0xE) < 4U))) {
            var_s2 = 1;
        }
        if (var_s2 != 0) {
            goto block_21;
        }
        goto block_22;
    }
block_21:
    if (g_MenuItemActionDisabled != 0) {
block_22:
        temp_s1->cursor_y = 1;
    }
}

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
            parent->appearance.cursorTargetX = parent->cursor_x;
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
