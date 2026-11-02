/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_record.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_MenuInventoryViewMode;
int g_MenuEquipSwapItem;
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
ItemDataRecord *g_MenuActionItemData;
int g_MenuEquipSwapSkipRestore;
int g_MenuItemContextFlag;
int g_MenuEquipSwapSource;
ItemDataRecord *g_MenuSelectedItemData;
int g_MenuItemDiscardMode;
int g_MenuSelectionLocked;
int g_MenuActionSubmenuOpen;
int g_MenuEquipMode;
int g_MenuLayoutLocked;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemSelectFlag;
int g_MenuInputPausePending;
int g_MenuMemCardConfirmPending;
MenuWidgetNode *g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;
int g_MenuActiveItemList;
int g_InvItemUsableFlag;
int g_McDialogMode;

#define NULL ((void *)0)

/* Builders that open the equip screen (weapon and armor lists) and the
 * inventory swap screen, the equip-mode getter and the equip list container
 * draw. Contiguous at 0x8004E704 (main_tu_evidence G0462). */

void Menu_OpenEquipScreen(int mode)
{
    MenuWidgetNode *parent, *container, *list;
    int count, offset;

    container = MenuWidget_FindByModeAndSelectedBase(2, 50);
    if (!container) {
        parent = MenuWidget_CreateSimpleNode(50, 0, 0, 0);
        container = MenuWidget_CreateNode(50, parent, parent);
        parent->update = Menu_StepInventoryCategory;
        container->draw = Menu_DrawEquipListContainer;
        if (!mode)
            Draw_SetPrimCallback(container, 3);
        MenuWidget_SetCurrentNode(container);
    }

    parent = MenuWidget_CreateSimpleNode(51, container, 0, 0);
    container = MenuWidget_CreateNode(51, parent, parent);
    parent->update = Menu_InventoryInputHandler;
    parent->disabled = 1;
    container->draw = Menu_DrawWeaponList;
    container->itemAction = Inv_RebuildWithSlotLimit;
    container->cursor_x = -1;
    if (mode)
        g_MenuEquipMode = 2;
    else
        g_MenuEquipMode = 1;

    count = Inv_StepScrollDisplay2(mode);
    BattleCmd_SyncActiveAmmo();
    Menu_SetBattleEquipMode(0);
    Draw_SetPrimCallback(container,
        Inv_TransferItemAlt2(mode ? 0x3803FE : 0xF400));

    offset = container->popup_node == 0;
    parent = MenuWidget_CreateSimpleNode(52, container, 0, 0);
    list = MenuWidget_CreateNode(52, parent, parent);
    parent->update = Menu_InventoryInputHandler;
    if (offset)
        MenuWidget_OffsetPosition(parent, -6, 0);
    list->draw = Menu_DrawArmorList;
    list->itemAction = Inv_RebuildWithSlotLimit;
    list->refreshItems = Menu_RebuildSelectableMask;
    list->cursor_x = -1;
    g_InvSwapTargetIndex = -1;
    g_InvSelectedItemIndex = -1;
    Draw_SetPrimCallback(list, count);
    list->linkedPrevious = container;
    container->linkedNext = list;

    if (!MenuWidget_FindByModeAndSelectedBase(1, 19)) {
        parent = MenuWidget_CreateSimpleNode(19, 0, 0, 0);
        parent->draw = Menu_DrawContextHelpText;
    }
    g_MenuActionSubmenuOpen = 0;
    g_MenuSelectionLocked = 0;
    MenuInput_InitQueue();
}

/* Reopen the equip screen with the current menu mode restored. */
void Menu_ReopenEquipScreen(void) {
    Menu_OpenEquipScreen(g_MenuEquipMode - 1);
}

int Menu_GetEquipMode(void) {
    return g_MenuEquipMode;
}

void Menu_OpenInventoryScreen(void) {
    s32 count;
    s32 bits;
    s32 loop_counter;
    s32 equip_entry;
    s32 raw;
    MenuWidgetNode *root0;
    MenuWidgetNode *root1;
    MenuWidgetNode *node;
    MenuWidgetNode *current;
    Inv_ResetActiveList();
    g_MenuEquipMode = 0;

    if (g_MenuEquipSwapSource != 0) {
        MenuWidget_InitPool();

        if (MenuWidget_FindByModeAndSelectedBase(1, 0) == 0) {
            root0 = MenuWidget_CreateSimpleNode(0, 0, 0, 0);
            node = MenuWidget_CreateNode(0, root0, root0);
            root0->update = Menu_StepItemSelectScreen;
            node->draw = Menu_DrawItemList;
            MenuWidget_SetCurrentNode(node);

            if (Menu_GetBattleEquipMode() != 0) {
                raw = g_MenuItemContextFlag;
                bits = raw & 0x1F;
            } else {
                raw = g_MenuItemContextFlag;
                bits = raw & 0x1EF;
            }

            count = 0;
            loop_counter = 8;
            do {
                count += bits & 1;
                loop_counter--;
                bits >>= 1;
            } while (loop_counter >= 0);

            Draw_SetPrimCallback(node, count);
            g_MenuSelectionLocked = 0;
            g_MenuBattleStatusOverlayActive = 1;
            Menu_CreateBonusPointAllocationView();
            Menu_CreateContextHelpPanel();
        }

        MenuWidget_NavScrollTo(0x2D);
        MenuWidget_NavScrollTo(0x18);
        MenuWidget_NavScrollTo(0x12);

        current = MenuWidget_GetCurrentNode();
        MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0)->draw = Menu_DrawAmmoTypeHeader;
        root1 = MenuWidget_CreateSimpleNode(1, current, 0, 0);
        node = MenuWidget_CreateNode(1, root1, root1);
        root1->update = Menu_InventoryInputHandler;
        node->draw = Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(node);
        node->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
        node->refreshItems = Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();

        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(node, Inv_GetAyaSlotLimit());
        g_MenuActionSubmenuOpen = 0;

        if (g_MenuEquipSwapSource != 0) {
            equip_entry = g_MenuEquipSwapSource - 1;
            node->cursor_x = equip_entry & 1;
            node->cursor_y = (equip_entry >> 1) & 0x7F;
            node->scroll_y = equip_entry >> 8;
        }

        if (g_MenuEquipSwapSkipRestore != 0) {
            g_MenuEquipSwapSkipRestore = 0;
        } else {
            g_AyaInventoryItems[g_MenuEquipSwapSource - 1] = g_MenuEquipSwapItem;
            Inv_RebuildSelectableMask();
        }

        g_MenuEquipSwapSource = 0;
    } else {
        Inv_SetActiveList(9, 0);
    }
}

void Menu_DrawEquipListContainer(void *node) {
    int count;

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawEquipSlotListItem);
    Draw_SetTextDimmed(1);

    count = ((MenuWidgetNode *)node)->visible_rows;
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}

/* Weapon and armor list draws, the inventory category step and the battle
 * command list draw. */

void Menu_DrawBattleCommandItem(int index);

void Menu_DrawWeaponList(MenuWidgetNode *node) {
    Inv_SelectActiveList(0);
    MenuWidget_DrawList(node, Menu_DrawInventoryItemRow);
}

void Menu_DrawArmorList(MenuWidgetNode *node) {
    Inv_SelectActiveList(1);
    MenuWidget_DrawList(node, Menu_DrawArmorItemRow);
}

int Menu_StepInventoryCategory(MenuWidgetNode *node, unsigned int flags)
{
    int selected, mode;
    MenuWidgetNode *child, *parent;
    node = MenuWidget_GetChild(node, 0);
    if (flags & 0x10000) {
        selected = MenuWidget_GridCellIndex(node);
        switch (g_MenuEquipMode == 1 && selected == 2 ? 4 : selected) {
        case 0:
            node = MenuWidget_FindByModeAndSelectedBase(2, 51);
            node->cursor_x = 0;
            MenuWidget_SetCurrentNode(node);
            Menu_PlayConfirmSound();
            break;
        case 1:
            parent = MenuWidget_CreateSimpleNode(59, node, 0, 0);
            child = MenuWidget_CreateNode(59, parent, parent);
            parent->update = Menu_EquipOptionsInputHandler;
            child->draw = Menu_DrawEquipOptionsList;
            MenuWidget_SetCurrentNode(child);
            MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 51));
            g_MenuInventoryViewMode = g_MenuEquipMode;
            break;
        case 2:
            MenuWidget_NavScrollTo(51);
            MenuWidget_NavScrollTo(52);
            g_MenuLayoutLocked = 1;
            Inv_SelectActiveList(0);
            g_StatBaseTable[0] = Inv_CountByCategory(14) ? 999 : Inv_CountByCategory(12);
            g_StatBaseTable[1] = Inv_CountByCategory(15) ? 999 : Inv_CountByCategory(13);
            if (g_MenuEquipMode) {
                g_StatBaseTable[2] = Inv_CountWayneStorageItemType(14) ? 999 : Inv_CountWayneStorageItemType(12);
                g_StatBaseTable[3] = Inv_CountWayneStorageItemType(15) ? 999 : Inv_CountWayneStorageItemType(13);
            } else {
                g_StatBaseTable[3] = 0;
                g_StatBaseTable[2] = 0;
            }
            Menu_StepInventoryRoot(0x33E, -1, -1);
            Menu_PlayConfirmSound();
            break;
        case 3:
            MenuWidget_NavScrollTo(51);
            MenuWidget_NavScrollTo(52);
            g_MenuItemUseMode = 1;
            Menu_StepInventoryRoot(0x37E, -1, -1);
            Menu_PlayConfirmSound();
            break;
        case 4:
            if (flags & 0x10000) {
                Menu_OpenInventoryScreen();
                Menu_PlayConfirmSound();
            }
            break;
        }
    } else if (flags & 0x40) {
        Menu_OpenInventoryScreen();
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawBattleCommandList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawBattleCommandItem);
}

int Menu_InventoryPageNavHandler(MenuWidgetNode *root, unsigned int flags) {
    MenuWidgetNode *node;
    MenuWidgetNode *other;
    /* Match debt: retail keeps the x shift in s2 and the target node in s1.
     * The pin-free form of Menu_AlignEquipPanels gives the shift the
     * higher global-alloc priority here and swaps them (lev 12). */
    register int offset asm("$18");
    int present;
    MenuWidgetNode *current;
    MenuWidgetNode *child;
    register int x asm("$2");
    register int oldx asm("$5");
    MenuWidget_GetChild(root, 0);
    if (flags & 0x14000) {
        if (Inv_GetPackedListCount() != 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 7);
            present = node != 0;
            if (node == 0) {
                node = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
                other = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
                child = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
                oldx = other->x;
                x = g_MenuItemUseMode;
                if (x == 0) {
                    x = (int)child->popup_node;
                    if (x == 0) x = 0x9C;
                    else x = 0xA2;
                } else x = 0xB0;
                offset = x - oldx;
                /* Keep the subtraction ahead of the first call argument. */
                asm volatile("" : : "r"(offset));
                MenuWidget_OffsetPosition(other, offset, 0);
                MenuWidget_ClearCursorY(other);
                other = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
                MenuWidget_OffsetPosition(other, offset, 0);
                MenuWidget_ClearCursorY(other);
                other = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
                MenuWidget_OffsetPosition(other, offset, 0);
                MenuWidget_ClearCursorY(other);
            }
            node->cursor_x = 0;
            node->cursor_y = 0;
            MenuWidget_ClampScroll(node);
            MenuWidget_SetCurrentNode(node);
            if (present) {
                root = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                MenuWidget_CreateNode(0x35, root, root)->draw = Menu_SetupSkillSubmenu;
            }
            Menu_PlayConfirmSound();
        } else {
            Menu_PlayErrorSound();
        }
    }
    if (flags & 0x40) {
        MenuWidget_DestroyNode(root);
        if (MenuWidget_FindByModeAndSelectedBase(1, 5)) {
            MenuWidget_NavScrollTo(5);
            MenuWidget_NavScrollTo(6);
            MenuWidget_NavScrollTo(7);
            MenuWidget_NavScrollTo(0x35);
            Menu_StepInventoryRoot(0x33E, -1, -1);
        } else {
            MenuWidget_NavScrollTo(0xF);
            MenuWidget_NavScrollTo(0xB);
            MenuWidget_NavScrollTo(0xD);
            MenuWidget_NavScrollTo(0x18);
            MenuWidget_NavScrollTo(0x30);
            if (g_MenuEquipMode) {
                MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
            } else {
                current = MenuWidget_GetCurrentNode();
                if (current) MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
                MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
            }
            g_MenuItemRenameMode = 0;
            MenuWidget_NavScrollTo(0x2F);
            g_MenuLayoutLocked = 0;
            g_MenuItemUseMode = 0;
            Menu_OpenEquipScreen(g_MenuEquipMode - 1);
        }
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawSkillSelectionList(int arg0);

void Menu_OpenSkillSelectionView(void) {
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_v0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x40, MenuWidget_GetCurrentNode(), 0, 0);
    temp_a0 = MenuWidget_CreateNode(0x40, temp_v0, temp_v0);
    temp_v0->update = Menu_HandleAyaInventorySelectionInput;
    temp_a0->draw = Menu_DrawSkillSelectionList;
    MenuWidget_SetCurrentNode(temp_a0);
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
}

void Menu_DrawSkillItem(int arg0);

void Menu_DrawSkillSelectionList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawSkillItem);
}

/* Equipment swap: picking the Aya inventory item to exchange records the
 * swap source; returning from the equip screen reopens the inventory. */

s32 Menu_HandleAyaInventorySelectionInput(s32 arg0, s32 arg1)
{
    s32 temp_v0_2;
    void *temp_v0;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_FindByModeAndSelectedBase(2, 1);
        g_MenuEquipSwapItem = g_AyaInventoryItems[MenuWidget_GridCellIndex(temp_v0)];
        temp_v0_2 = MenuWidget_GridCellIndex(temp_v0) | (((MenuWidgetNode *)temp_v0)->scroll_y << 8);
        g_MenuEquipSwapSource = temp_v0_2 + 1;
        Inv_RemoveActiveListItem(temp_v0_2 & 0xFF);
        MenuWidget_DestroyNode(arg0);
        MenuWidget_NavScrollTo(0x1B);
        MenuWidget_NavScrollTo(1);
        MenuWidget_NavScrollTo(0);
        Menu_OpenEquipScreen(MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0)));
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0));
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_SetSwapReturnFlag(void) {
    if (g_MenuEquipSwapSource != 0) {
        g_MenuEquipSwapSkipRestore = 1;
    }
}

void Menu_ProcessSwapReturnIfPending(void) {
    if (g_MenuEquipSwapSkipRestore != 0) {
        Menu_OpenInventoryScreen();
        g_MenuEquipSwapSkipRestore = 0;
    }
}

/* Item use/discard panel: creation, its input handler and the item name
 * header. Defines the selected item record and the discard flag. */

void Menu_DrawEquipStatsPanel(void);
int Menu_HandleItemInput(int arg0, int arg1);
void Menu_DrawEquipItemName(void);
void Menu_DrawEquipItemIcon(void);
void Menu_DrawSoundEffectList(MenuWidgetNode *node);
void Menu_DrawEquipItemDetailPanel(MenuWidgetNode *node);
void Draw_PrintRawText(u8 *arg0);

void Menu_CreateItemUsePanel(int itemId) {
    s32 width;
    MenuWidgetNode *node;
    MenuWidgetNode *created;
    ItemDataRecord *data;
    ItemDataRecord *temp_v0;

    temp_v0 = Inv_LookupData(itemId);
    g_MenuSelectedItemData = temp_v0;
    if (temp_v0->kind < 0xAU) {
        node = MenuWidget_CreateSimpleNode(5, 0, 0, 0);
        created = MenuWidget_CreateNode(5, node, node);
        node->draw = &Menu_DrawEquipStatsPanel;
        node->update = &Menu_HandleItemInput;
        width = 0x80;
        node->grid_width = width;
        MenuWidget_OffsetPosition(node, 0x44, 0x14);
        created->draw = &Menu_DrawSoundEffectList;
        MenuWidget_ClearColumnLayout(created);
        created->aux_index = -1;
        MenuWidget_SetCurrentNode(node);
        node = MenuWidget_CreateSimpleNode(6, 0, 0, 0);
        node->grid_width = width;
        MenuWidget_OffsetPosition(node, 0x44, 0x14);
        created = MenuWidget_CreateNode(6, node, node);
        created->draw = &Menu_DrawEquipItemDetailPanel;
        MenuWidget_ClearColumnLayout(created);
        data = g_MenuSelectedItemData;
        created->draw_state = 0x3E;
        Draw_SetPrimCallback(created, data->tailCount);
        g_InvItemUsableFlag = g_MenuSelectedItemData->kind != 9;
    } else {
        node = MenuWidget_CreateSimpleNode(0x37, 0, 0, 0);
        node->draw = &Menu_DrawEquipItemIcon;
        node->update = &Menu_HandleItemInput;
        MenuWidget_SetCurrentNode(node);
    }
    MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = &Menu_DrawEquipItemName;
    MenuInput_InitQueue();
}

void Menu_DrawEquipStatsPanel(void) {
    ItemDataRecord *temp_s0;

    Menu_DrawEquipStatsDelta(g_MenuSelectedItemData);
    temp_s0 = g_MenuSelectedItemData;
    if (temp_s0 != NULL) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(temp_s0->baseStats[2]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(temp_s0->bonusStats[2]);
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(temp_s0->baseStats[1]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(temp_s0->bonusStats[1]);
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(temp_s0->baseStats[0]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(temp_s0->bonusStats[0]);
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }
}

int Menu_HandleItemInput(int arg0, int arg1) {
    int value;
    ItemDataRecord *ptr;

    if ((arg1 & 0x10040) != 0) {
        if (g_MenuItemDiscardMode != 0) {
            ptr = g_MenuSelectedItemData;
            g_MenuItemDiscardMode = 0;
            value = ptr->itemId;
            Inv_SetActiveList(0, &value);
        } else {
            Inv_SetActiveList(9, 0);
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_DrawEquipItemName(void) {
    Draw_OffsetCursor(4, 4);
    Draw_PrintRawText(Text_LookupTableC(g_MenuSelectedItemData->itemId - 1));
}

void Menu_DrawEquipItemIcon(void) {
    Draw_OffsetCursor(6, 6);
    Menu_CursorRenderData(g_MenuSelectedItemData);
}

void Menu_ResetStateFlags(void) {
    g_MenuLayoutLocked = 0;
    g_MenuItemUseMode = 0;
    g_MenuItemSelectFlag = 0;
    g_MenuSelectionLocked = 0;
    g_MenuActionSubmenuOpen = 0;
    g_MenuEquipMode = 0;
    g_MenuItemRenameMode = 0;
    g_MenuItemDiscardMode = 0;
    g_MenuInputPausePending = 0;
    g_MenuMemCardConfirmPending = 0;
}

/* List renderers for the item, selectable equip-slot and context-action
 * lists, the action-option and equip-slot adapters, and the usable-item and
 * skill action-code lists. */

void Menu_DrawItemSlot(int index);
void Menu_DrawSelectableEquipSlotRow(int index);
void Menu_ContextPick(int index);
void Menu_DrawActionOptionRow(int index);
void Menu_SoundTestSelect(int index);
void Menu_DrawActionCodeItem(int index);
void Draw_SetTextDimmed(int value);
void Battle_DrawActiveStatus(void);

void Menu_DrawItemList(MenuWidgetNode *node) {
    int count;

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawItemSlot);
    Draw_SetTextDimmed(1);

    count = node->visible_rows;
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }

    if (Battle_GetStateFlag1() == 0) {
        if (g_MenuBattleStatusOverlayActive != 0) {
            Battle_DrawActiveStatus();
        }
    }
}

void Menu_DrawSelectableEquipSlotList(MenuWidgetNode *node) {
    Inv_SelectActiveList(0);
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSelectableEquipSlotRow);
}

void Menu_DrawContextActionList(MenuWidgetNode *node) {
    MenuWidgetNode *saved = node;

    Inv_SelectActiveList(g_MenuActiveItemList);
    MenuWidget_DrawList(saved, Menu_ContextPick);
}

void Menu_DrawActionOptionList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawActionOptionRow);
}

void Menu_DrawSoundTestList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_SoundTestSelect);
}

void Menu_DrawUsableItemActionList(MenuWidgetNode *node) {
    ItemDataRecord *ptr;

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(0));
    g_MenuActionItemData = ptr;
    g_InvItemUsableFlag = ptr->kind != 9;
    Draw_SetPrimCallback(node, ptr->tailCount);
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawActionCodeItem);
}

void Menu_SetupSkillSubmenu(MenuWidgetNode *list) {
    MenuWidgetNode *node;
    MenuWidgetNode *other;
    int index;
    ItemDataRecord *ptr;

    node = MenuWidget_FindByModeAndSelectedBase(1, 7);
    other = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
    Inv_SelectActiveList(MenuWidget_GridCellIndex(other) == 1);

    if (node != 0 && node->cursor_y == 0) {
        index = MenuWidget_GridCellIndex(MenuWidget_GetChild(node, 0));
        if (index >= 0) {
            other = MenuWidget_FindByModeAndSelectedBase(2, 7);
            index = Inv_GetPackedListItem(MenuWidget_GridCellIndex(other));
            Inv_RememberSelection(1, index);
        }
    }

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(1));
    g_MenuActionItemData = ptr;
    Draw_SetPrimCallback(list, ptr->tailCount);
    g_MenuActiveWidget = list;
    MenuWidget_DrawList(list, Menu_DrawActionCodeItem);
}

void Menu_DrawUsableItemActionList2(MenuWidgetNode *node) {
    ItemDataRecord *ptr;

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(1));
    g_MenuActionItemData = ptr;
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawActionCodeItem);
}

void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
void Menu_HandleInput(int index);

void Menu_DrawEquipSelectionList(MenuWidgetNode *node) {
    int mask;
    int excluded;
    int index;
    int type;

    index = MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 0x36));
    if (index >= 0) {
        if (node->selected_base == 7) {
            mask = 0x33E;
            excluded = Inv_RestoreSelection(0);
            type = Inv_GetActiveListItemType(excluded);
            if (type == 0 || type >= 6) {
                mask = 1 << type;
            } else {
                mask = 0x3E;
            }

            if (Inv_IsActiveListOverrideSelected() != index) {
                excluded = -1;
            }
        } else {
            mask = 0x33E;
            excluded = -1;
        }

        Inv_SelectActiveList(index);
        Inv_BuildFilteredPackedListExcluding(mask, excluded);
        Draw_SetPrimCallback(node, Inv_GetPackedListCount());
    }

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_HandleInput);
}

/* Item-slot availability, modifier, armor and memory-card port list draw
 * callbacks, and the port and save-slot selectability filters. */

void Draw_AllocColorQuad(int arg0, int arg1);
void Menu_DrawModNameItem(int index);
void Menu_DrawArmorListItem(int arg0);
void Menu_DrawArmorEquipItem(int arg0);
void Menu_DrawMemCardPortItem(int arg0);

int Menu_GetItemSlotAvailability(int arg0) {
    if (arg0 < Inv_GetPackedListCount()) {
        return Menu_CheckItemAffordable(Inv_GetPackedListItem(arg0));
    }

    return 0;
}

void Menu_DrawEquipModList(MenuWidgetNode *arg0) {
    int ammo;
    MenuWidgetNode *saved_arg;
    int remaining;
    int item;

    saved_arg = arg0;
    item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(arg0));
    remaining = BattleCmd_GetRemainingAmmo(&ammo);
    {
        int highlighted = Inv_GetSlotHighlightState(item, remaining);
        remaining -= highlighted;
    }
    Draw_AllocColorQuad(remaining, ammo);
    MenuWidget_DrawList(saved_arg, Menu_DrawModNameItem);
}

void Menu_DrawArmorListContainer(MenuWidgetNode *node) {
    int count;

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawArmorListItem);
    Draw_SetTextDimmed(1);
    count = node->visible_rows;
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}

void Menu_DrawArmorSelectionList(MenuWidgetNode *node) {
    Inv_SelectActiveList(1);
    MenuWidget_DrawList(node, Menu_DrawArmorEquipItem);
}

int Menu_IsMemCardSlotSelectable(int arg0) {
    int ret;

    ret = 0;
    if (arg0 == 2) {
        ret = 1;
    } else if (MemCard_IsPortPresent(arg0) != 0) {
        ret = 1;
    }
    return ret;
}

void Menu_DrawMemCardPortList(MenuWidgetNode *node) {
    int count;

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawMemCardPortItem);
    Draw_SetTextDimmed(1);
    count = node->visible_rows;
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}

int Menu_IsMemCardSlotCompatible(int arg0) {
    MemCardSaveSlot *ptr;
    int result;

    if (g_McDialogMode == 0) {
        ptr = MemCard_GetSlot(g_MenuActiveWidget->selected_base - 0x25, arg0);
        result = ptr->state == MEMCARD_SLOT_OCCUPIED && ptr->titleStyleFlag == Save_GetTitleStyleFlag();
    } else {
        result = 1;
    }

    return result;
}
