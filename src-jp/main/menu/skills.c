/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/save_tail_state.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_MenuItemPrimListHandles[2];
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
int g_MenuActionSubmenuOpen;
int g_MenuActionTextBase;
int g_MenuActiveItemList;
MenuConfirmCallback g_MenuConfirmCallback;
int g_MenuEquipMode;
int g_MenuEquipSwapSource;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSelectedSpell;

int g_MenuInputPausePending;

void Menu_DeferredExec(void) {
    if (g_MenuInputPausePending != 0) {
        if ((g_DiscChangeFlags & 3) == 0) {
            MenuInput_SetPollingPaused(0);
            g_MenuInputPausePending = 0;
        }
    }
}

#define NULL ((void *)0)
void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
int Menu_StepEquipConfirm(MenuWidgetNode *node, int input);
void Menu_DrawEquipSelectionList(MenuWidgetNode *node);

void Menu_StepSkillList(MenuWidgetNode *arg0, s32 arg1) {
    M2C_UNK var_a0_2;
    MenuWidgetNode *saved_arg0 = arg0;
    s32 saved_arg1 = arg1;
    s32 temp_v0;
    s32 mask;
    MenuWidgetNode *child;
    MenuWidgetNode *parent;

    if (g_MenuLayoutLocked != 0) {
        temp_v0 = Inv_RestoreSelection(0);
        if ((Inv_GetActiveListItemType(temp_v0) != 0) && (Inv_GetActiveListItemType(temp_v0) < 6)) {
            mask = 0x3E;
        } else {
            mask = 1 << Inv_GetActiveListItemType(temp_v0);
        }
        Inv_BuildFilteredPackedListExcluding(mask, temp_v0);
        parent = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
        MenuWidget_OffsetPosition(parent, 0xB4 - parent->x, 0xA4 - parent->y);
    } else {
        var_a0_2 = 0x200;
        if (g_InvItemUsableFlag != 0) {
            var_a0_2 = 0x1FE;
        }
        Inv_BuildFilteredPackedList(var_a0_2);
    }
    if ((g_MenuEquipMode != 0) || (Inv_GetPackedListCount() != 0)) {
        parent = MenuWidget_CreateSimpleNode(7, saved_arg0, 0, 0);
        child = MenuWidget_CreateNode(7, parent, parent);
        parent->update = Menu_StepEquipConfirm;
        {
            s32 temp_v1 = g_MenuLayoutLocked;
            child->draw = &Menu_DrawEquipSelectionList;
            if (temp_v1 != 0) {
                MenuWidget_ClearColumnLayout(child);
            }
        }
        if (g_InvItemUsableFlag == 0) {
            MenuWidget_SetColumnLayout(child, 0x13);
        }
        if (saved_arg1 == 0) {
            child->cursor_x = -1;
        } else {
            child->cursor_x = 0;
        }
        if (child->cursor_x >= 0) {
            MenuWidget_SetCurrentNode(child);
        }
        Draw_SetPrimCallback(child, Inv_GetPackedListCount());
        if (g_MenuEquipMode != 0) {
            child->cursor_x = -1;
            MenuWidget_OffsetPosition(parent, 0, 0x14);
        }
        Menu_PlayConfirmSound();
        return;
    }
    Menu_PlayErrorSound();
}
#undef NULL

/* Confirmation input handling; the equip callback (Menu_OnEquipConfirm) lives in another unit in JP. */

int Menu_StepEquipConfirm(MenuWidgetNode *node, int input) {
    MenuWidgetNode *child;
    MenuWidgetNode *parent;
    MenuWidgetNode *dialog;
    MenuWidgetNode *option_node;
    MenuWidgetNode *next;
    int item;
    int handled;
    int allowed;
    int width;
    u8 *dialog_text;
    MenuConfirmCallback callback;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);
    parent = child;

    if ((input & 0x10000) != 0) {
        item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(parent));
        if (g_MenuLayoutLocked != 0) {
            Inv_SelectActiveList(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 0x36)) == 1);

            allowed = 0;
            if ((g_StatBaseTable[0] + g_StatBaseTable[1] + g_StatBaseTable[2] + g_StatBaseTable[3]) != 0) {
                unsigned mask = Inv_LookupActiveListData(item)->flags & 0x40;
                allowed = mask < 1U;
            }

            if (allowed == 0) goto error;

            Inv_RememberSelection(1, item);
            MenuWidget_NavScrollTo(0x36);
            Menu_ConfigureScreen();
        } else {
            goto reserved;
        }
confirm:
        handled = 1;
        Menu_PlayConfirmSound();
        goto done;
reserved:
        if (Inv_IsAyaInventorySlotReserved(item) == 0) {
        if (Menu_GetBattleEquipMode() != 0) {

        dialog_text = Inv_LookupActiveListDisplayData(item);
        dialog = MenuWidget_CreateSimpleNode(0x29, parent, 0, 1);
        option_node = MenuWidget_CreateNode(0x29, dialog, dialog);
        dialog->draw = Menu_DrawItemLabel;
        dialog->update = Menu_ConfirmDialogHandler;
        option_node->draw = Menu_DrawActionOptionList;
        g_MenuActionTextBase = 5;
        MenuWidget_SetCurrentNode(option_node);
        callback = Menu_OnEquipConfirm;
        Inv_SelectActiveList(g_MenuActiveItemList);

        if (dialog_text != 0) {
            Text_CopyFFTerminatedBytes(g_MenuItemLabelBuffer, dialog_text);
        } else {
            g_MenuItemLabelBuffer[0] = 0xFF;
        }

        {
            u8 *suffix;
            register u8 *text_buf asm("$4");

            suffix = Text_LookupTable4(0x71);
            text_buf = g_MenuItemLabelBuffer;
            Text_AppendFFTerminatedBytes(text_buf, suffix);
            g_MenuItemPrimListHandles[0] = 0;
            text_buf = g_MenuItemLabelBuffer;
            width = Draw_MeasureTextWidth(text_buf);
            if (width < 0x78) {
                width = 0x78;
            } else {
                text_buf = g_MenuItemLabelBuffer;
                width = Draw_MeasureTextWidth(text_buf);
            }
        }

        dialog->grid_width = width + 0x14;
        dialog->visible_rows = 0x32;
        dialog->x = (0x12C - width) >> 1;
        option_node->x = (dialog->grid_width - 0x80) >> 1;
        {
            int height = dialog->visible_rows;
            g_MenuConfirmCallback = callback;
            option_node->y = height - 0x14;
        }
        goto confirm;
        }
simple_confirm:
        /* JP runs the confirm callback body in place, without rebuilding the packed list. */
        parent = MenuWidget_FindByModeAndSelectedBase(2, 7);
        if (g_InvItemUsableFlag) {
            Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(parent));
            parent->cursor_x = -1;
            child = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(child->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : child);
            MenuWidget_NavScrollTo(0x35);
        } else if (!Inv_GetSlotItemData(MenuWidget_GridCellIndex(parent))) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            parent->cursor_x = -1;
            child = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(child->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : child);
            MenuWidget_NavScrollTo(0x35);
        }
        parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (parent == MenuWidget_GetCurrentNode()) {
            parent->cursor_y = 0;
            parent->cursor_x = 0;
        }
        if (g_InvItemUsableFlag) {
            MenuInput_SetPollingPaused(1);
            g_MenuInputPausePending = 1;
        }
        goto confirm;
        }
error:
        handled = 1;
        Menu_PlayErrorSound();
        goto done;
    } else if ((input & 0x1040) != 0) {
        if (g_MenuEquipMode != 0) {
            MenuWidget_NavScrollTo(0x35);
            parent->cursor_x = -1;
            parent->scroll_y = 0;
            MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x36));
        } else {
            if ((input & 0x40) == 0) {
                handled = 1;
                goto done;
            }

            MenuWidget_NavScrollTo(0x35);
            if (g_MenuLayoutLocked != 0) {
                MenuWidget_DestroyNode(node);
                MenuWidget_NavScrollTo(5);
                MenuWidget_NavScrollTo(6);
                MenuWidget_NavScrollTo(0x1C);
                MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
                Menu_StepInventoryRoot(0x33E, -1, -1);
            } else {
                child->cursor_x = -1;
                parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
                if (parent == MenuWidget_GetCurrentNode()) {
                    int hidden = ((unsigned)parent->has_scroll < 1U);
                    next = parent;
                    next->cursor_x = hidden;
                } else {
                    next = MenuWidget_FindByModeAndSelectedBase(2, 5);
                }
                MenuWidget_SetCurrentNode(next);
            }
        }

        handled = 1;
        Menu_PlayCancelSound();
    }
done:
    return handled;
}

void Menu_CreateSkillActionScreen(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(8, arg0, 0, 0);
    child = MenuWidget_CreateNode(8, parent, parent);
    parent->update = Menu_SkillActionHandler;
    child->draw = Menu_DrawEquipModList;
    child->selectionAvailable = Menu_GetItemSlotAvailability;
    MenuWidget_SetCurrentNode(child);
    Item_PackParasiteSpells();
    Draw_SetPrimCallback(child, Inv_GetPackedListCount());
}

void Menu_StepItemDetailPanel2(void);

int g_MenuSelectedSpell;

s32 Menu_SkillActionHandler(MenuWidgetNode *node, s32 flags) {
    MenuWidgetNode *child;
    s32 handled;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x10000) {
        if (MenuWidget_GetCellIndex(child) >= 0) {
            handled = 1;
            Menu_StepItemDetailPanel2();
            Menu_PlayConfirmSound();
        } else {
            handled = 1;
            Menu_PlayErrorSound();
        }
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(node);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1D));
        Menu_CreateBonusPointAllocationView();
        handled = 1;
        Menu_PlayCancelSound();
    }
    return handled;
}

void Menu_StepItemDetailPanel2(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *current;
    u8 *label;
    MenuWidgetNode *option_node;
    int width;
    MenuConfirmCallback callback;
    int height;

    g_MenuSelectedSpell = Inv_GetPackedListItem(MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 8)));
    current = MenuWidget_GetCurrentNode();
    label = Text_LookupTable8(g_MenuSelectedSpell + 0xEB);
    node = MenuWidget_CreateSimpleNode(0x29, current, 0, 1);
    option_node = MenuWidget_CreateNode(0x29, node, node);
    node->draw = Menu_DrawItemLabel;
    node->update = Menu_ConfirmDialogHandler;
    option_node->draw = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(option_node);
    callback = Menu_OnBattleCommandConfirm;
    Inv_SelectActiveList(g_MenuActiveItemList);

    if (label != 0) {
        Text_CopyFFTerminatedBytes(g_MenuItemLabelBuffer, label);
    } else {
        g_MenuItemLabelBuffer[0] = 0xFF;
    }

    {
        u8 *suffix;
        register u8 *text_buf asm("$4");

        suffix = Text_LookupTable4(0x1A);
        text_buf = g_MenuItemLabelBuffer;
        Text_AppendFFTerminatedBytes(text_buf, suffix);
        g_MenuItemPrimListHandles[0] = 0;
        text_buf = g_MenuItemLabelBuffer;
        width = Draw_MeasureTextWidth(text_buf);
        if (width < 0x78) {
            width = 0x78;
        } else {
            text_buf = g_MenuItemLabelBuffer;
            width = Draw_MeasureTextWidth(text_buf);
        }
    }

    node->grid_width = width + 0x14;
    node->visible_rows = 0x32;
    node->x = (0x12C - width) >> 1;
    option_node->x = (node->grid_width - 0x80) >> 1;
    height = node->visible_rows;
    g_MenuConfirmCallback = callback;
    option_node->y = height - 0x14;
}

void Menu_OnBattleCommandConfirm(MenuWidgetNode *arg0, int arg1) {
    if (arg1 != 0) {
        if (Menu_GetBattleEquipMode() != 0) {
            MenuWidget_NavScrollTo(8);
        }
        Battle_UseItem(g_MenuSelectedSpell);
    }
}

void Menu_CreateInvSwapView(MenuWidgetNode *parent, int mode) {
    int mode_offset;
    MenuWidgetNode *node;
    MenuWidgetNode *current;

    mode_offset = mode;
    node = MenuWidget_CreateSimpleNode(mode_offset + 0x3A, parent, 0, 0);
    current = MenuWidget_CreateNode(mode_offset + 0x3A, node, node);
    node->update = Menu_EquipOptionsInputHandler;
    current->draw = Menu_DrawEquipOptionsList;
    MenuWidget_SetCurrentNode(current);
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));

    if (mode_offset != 0) {
        u32 value;

        value = (unsigned int)g_MenuEquipMode;
        g_MenuInventoryViewMode = value;
    } else {
        g_MenuInventoryViewMode = 0;
    }
}

#define NULL ((void *)0)

void Menu_DrawEquipSortToggleList(int node);
int Menu_EquipGridHandler(void *node, int flags);
void Menu_CreateEquipInfoPanel(int parent, unsigned int option);
int Menu_EquipSelectInput(void *node, unsigned int flags);
void Menu_DrawEquipInfoOptionList(int node);
void Menu_DrawWeaponModSlots(s32 index);

int g_MenuInventoryViewMode;
int g_InvItemUsableFlag;

void Menu_OpenInventoryOrSwapView(s32 showList) {
    s32 mode;
    s32 source;
    MenuWidgetNode *current;
    MenuWidgetNode *panel;
    MenuWidgetNode *list;

    MenuWidget_NavScrollTo(0x3C);
    MenuWidget_NavScrollTo(0x3B);
    MenuWidget_NavScrollTo(0x3A);
    mode = g_MenuInventoryViewMode;
    switch (mode) {
    case 0:
        if (showList != 0) {
            current = MenuWidget_GetCurrentNode();
            current->cursor_y = 0;
            MenuWidget_CreateSimpleNode(0x1B, NULL, 0, 0)->draw = &Menu_DrawAmmoTypeHeader;
            panel = MenuWidget_CreateSimpleNode(1, current, 0, 0);
            list = MenuWidget_CreateNode(1, panel, panel);
            panel->update = Menu_InventoryInputHandler;
            list->draw = Menu_DrawSelectableEquipSlotList;
            MenuWidget_SetCurrentNode(list);
            list->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
            list->refreshItems = Menu_RebuildSelectableMask;
            Inv_RebuildSelectableMask();
            g_InvSwapTargetIndex = -1;
            g_InvSelectedItemIndex = -1;
            Draw_SetPrimCallback(list, Inv_GetAyaSlotLimit());
            source = mode = g_MenuEquipSwapSource;
            g_MenuActionSubmenuOpen = 0;
            if (source != 0) {
                source -= 1;
                list->cursor_x = source & 1;
                list->cursor_y = (source >> 1) & 0x7F;
                list->scroll_y = source >> 8;
            }
        } else {
            Menu_CreateBonusPointAllocationView();
        }
        break;
    case 1:
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));
        break;
    case 2:
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));
        break;
    }
}

void Menu_DrawWeaponModSlots(s32 arg0) {
    s32 temp_s0;
    u8 var_a0;

    temp_s0 = arg0 + g_MenuModSlotIconIndexOffsetByView[g_MenuInventoryViewMode];
    if (g_MenuInventoryViewMode == 1) {
        Draw_OffsetCursor(0, 2);
        Draw_AllocSprite(g_MenuModSlotFirstGlyphs[temp_s0]);
        Draw_OffsetCursor(0x12, 2);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -2);
        Draw_AllocSprite(g_MenuModSlotSecondGlyphs[temp_s0]);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 2);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -2);
            var_a0 = g_MenuModSlotThirdGlyphs[temp_s0];
            Draw_AllocSprite(var_a0);
        }
    } else {
        Draw_AllocSprite(g_MenuModSlotFirstGlyphs[temp_s0]);
        Draw_AllocSprite(0x68U);
        Draw_OffsetCursor(0x12, 4);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -4);
        Draw_AllocSprite(g_MenuModSlotSecondGlyphs[temp_s0]);
        Draw_AllocSprite(0x68U);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 4);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -4);
            Draw_AllocSprite(g_MenuModSlotThirdGlyphs[temp_s0]);
            var_a0 = 0x68;
            Draw_AllocSprite(var_a0);
        }
    }
}

void Menu_DrawEquipOptionsList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawWeaponModSlots);
}

s32 Menu_EquipOptionsInputHandler(void *arg0, s32 arg1) {
    s32 temp_a1;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
    MenuWidgetNode *temp_v0_3;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetChild(arg0, 0);
        temp_a1 = MenuWidget_GridCellIndex(temp_v0);
        switch (g_MenuInventoryViewMode) {                       /* irregular */
        case 0:
            if (temp_a1 == 2) {
                temp_v0_2 = MenuWidget_CreateSimpleNode(0x3B, temp_v0, 0, 0);
                temp_v0_3 = MenuWidget_CreateNode(0x3B, temp_v0_2, temp_v0_2);
                temp_v0_2->update = &Menu_EquipGridHandler;
                MenuWidget_OffsetPosition(temp_v0_2, 0x70 - temp_v0_2->x, 0x48 - temp_v0_2->y);
                temp_v0_3->draw = &Menu_DrawEquipSortToggleList;
                MenuWidget_SetCurrentNode(temp_v0_3);
            } else {
            case 2:
                Menu_CreateEquipInfoPanel(temp_v0, temp_a1);
            }
            break;
        case 1:
            Inv_SortContainerItems(temp_a1);
            Menu_OpenInventoryOrSwapView(1);
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        Menu_OpenInventoryOrSwapView(0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawEquipSortToggleList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawEquipSortToggleRow);
}

int Menu_EquipGridHandler(void *arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (g_MenuInventoryViewMode != 0) {
            Inv_SortContainerItems(temp);
        } else {
            Inv_SortInventoryByMode(2, temp);
        }
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}

void Menu_CreateEquipInfoPanel(int arg0, unsigned int arg1) {
    MenuWidgetNode *node;
    MenuWidgetNode *child;

    node = MenuWidget_CreateSimpleNode(0x3C, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x3C, node, node);
    node->update = Menu_EquipSelectInput;
    child->draw = Menu_DrawEquipInfoOptionList;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuInventoryViewMode == 0) {
        MenuWidget_OffsetPosition(
            node,
            0x70 - node->x,
            0x20 - node->y
        );
    }

    g_InvItemUsableFlag = arg1 < 1;
    if (arg1 != 0) {
        MenuWidget_OffsetPosition(node, 0, 0x14);
    }
}

void Menu_DrawEquipInfoOptionList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawEquipInfoOptionRow);
}

int Menu_EquipSelectInput(void *arg0, unsigned int arg1) {
    int temp;
    void (*handler)(int, int);

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        handler = g_MenuInventoryViewMode ? Inv_SortInventoryItems : Inv_SortInventoryByMode;
        handler(g_InvItemUsableFlag, temp);
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}
