/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_nop.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointSpendCurrentValue;
int g_MenuItemPrimListHandles[2];
int g_MenuSourceItemSlot;
int g_MenuStatItemStatIndex;
int g_MenuSourceItemList;
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_BonusPointSpendStat;
int g_BonusPointSpendWorkingValue;
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
int g_MenuActionSubmenuOpen;
int g_MenuActiveItemList;
int g_MenuBattleStatusOverlayActive;
int g_MenuBonusPointBarAnimActive;
MenuConfirmCallback g_MenuConfirmCallback;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_MenuEquipMode;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSpendArrowDirection;

void Menu_CreateItemDetailView(MenuWidgetNode *arg0);
void Menu_CreateEquipStatsPanel(int arg0);

void Menu_ConfigureScreen(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *other;
    int value;

    MenuWidget_NavScrollTo(0x35);

    node = MenuWidget_FindByModeAndSelectedBase(1, 5);
    node->grid_width = 0x80;
    node = MenuWidget_FindByModeAndSelectedBase(1, 6);
    node->grid_width = 0x80;
    node = MenuWidget_FindByModeAndSelectedBase(2, 6);
    node->draw_state = 0x3E;

    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 7));

    node = MenuWidget_FindByModeAndSelectedBase(2, 0);
    if (node == 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
    }

    Menu_CreateItemDetailView(node);
    Menu_CreateEquipStatsPanel(node);

    node = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 5), 1);
    if (node != 0) {
        MenuWidget_SetCurrentNode(node);
        node->cursor_x = 0;
    }

    node = MenuWidget_FindByModeAndSelectedBase(2, 0x1B);
    other = MenuWidget_FindByModeAndSelectedBase(2, 0x1C);
    node->linkedNext = other;
    other->linkedPrevious = node;

    /* Slides the 0x1C tab down by one 16-pixel row per two mod slots of the
     * selected item, relative to its 0x7C home row. */
    {
        MenuWidgetNode *tab = MenuWidget_FindByModeAndSelectedBase(1, 0x1C);
        int rows = ((Inv_LookupActiveListData(Inv_RestoreSelection(0))->tailCount + 1) >> 1) * 16;
        int y = tab->y - 0x7C;
        MenuWidget_OffsetPosition(tab, 0, rows - y);
    }

    node = MenuWidget_FindByModeAndSelectedBase(2, 5);
    if (node != 0) {
        value = -1;
        node->cursor_x = value;
    }
}

/* Category tab reset and equip screen header, item action submenu,
 * skill/stat point use, the equip stats panel and the item detail and item
 * list views of the inventory menu. Contiguous at 0x80047678 and joined by
 * private data and helper calls. */

int Menu_EquipStatsInputHandler(MenuWidgetNode *node, int flags);
int Menu_DetailViewInput(MenuWidgetNode *arg0, unsigned int arg1);
int g_MenuSelectedItemIndex;
void Menu_CreateItemActionSubmenu(void);
void Menu_CreateItemList(void);
int g_MenuActionTextBase;
int g_InvItemUsableFlag;
int g_MenuSelectedItemList;
int g_MenuSelectedItemSlot;
void Menu_DrawActiveListSlot(int arg0);
#define NULL ((void *)0)
void Menu_DrawEquipStats(void);
/* Inlined into Menu_OpenItemList, like Menu_CreateItemActionSubmenu; no
 * out-of-line copy exists in retail. */
static inline void Menu_CreateScreenModeSubmenu(void) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x2c, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x2c, parent, parent);
    parent->draw = Menu_DrawEquipScreenHeader;
    parent->update = Menu_InventoryItemHandler;
    child->draw = Menu_DrawScreenModeList;
    child->selectionAvailable = Menu_GetStatBaseValue;
    if (g_MenuEquipMode != 0) {
        parent->visible_rows += 0x10;
        child->visible_rows = 2;
        child->y_limit = 2;
        child->x += 0x1c;
    }
    MenuWidget_SetCurrentNode(child);
}

void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_DrawItemListHeader(void);
int g_MenuItemListHeaderGlyph;

int g_MenuEquipMode;

void Draw_FlushPrimList(int arg0);
void Draw_SetColor(int arg0);

void Menu_InitCategoryTabs(void) {
    void *node;

    Inv_BuildStorageDisplay();
    MenuWidget_NavScrollTo(0x2C);
    MenuWidget_NavScrollTo(0xB);
    MenuWidget_NavScrollTo(0xA);
    MenuWidget_NavScrollTo(5);
    MenuWidget_NavScrollTo(6);
    MenuWidget_NavScrollTo(7);
    MenuWidget_NavScrollTo(0x1C);

    node = MenuWidget_FindByModeAndSelectedBase(2, 0);
    if (node == 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
    }

    MenuWidget_SetCurrentNode(node);
    Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
    Menu_PlayCancelSound();
}

void Menu_DrawEquipScreenHeader(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_FlushPrimList(0x69);

    if (g_MenuEquipMode != 0) {
        Draw_SetColor(0x78703C);
        Draw_OffsetCursor(0xA, 0x16);
        Draw_PrintTextById(0x1F);
        Draw_OffsetCursor(0, 0x10);
        Draw_PrintTextById(0x21);
        Draw_SetColor(0x808080);
    }
}

int Menu_InventoryItemHandler(MenuWidgetNode *arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GetCellIndex(MenuWidget_GetChild(arg0, 0));
        g_MenuSelectedItemIndex = temp;
        if (temp >= 0) {
            Menu_CreateItemActionSubmenu();
            Menu_PlayConfirmSound();
        } else {
            Menu_PlayErrorSound();
        }
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}

/* Also inlined into Menu_OpenItemList, which follows in this unit. */
inline void Menu_CreateItemActionSubmenu(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *child;

    node = MenuWidget_CreateSimpleNode(4, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(4, node, node);
    node->draw = Menu_DrawItemActionSubmenu;
    node->update = Menu_StepSkillSelect;
    child->draw = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuSelectedItemIndex & 1) {
        node->visible_rows -= 0x1C;
        child->y -= 0x1C;
    }
}

void Menu_DrawItemActionSubmenu(void) {
    int selected;
    int icon;
    u8 *entry;

    selected = Inv_RestoreSelection(g_MenuSelectedItemList);
    Draw_OffsetCursor(4, 4);
    Draw_StatePush();

    if (g_MenuSelectedItemSlot >= 0) {
        entry = Inv_LookupActiveListData(selected);
        entry += g_MenuSelectedItemSlot;
        icon = entry[0x15] & 0x1F;
        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(icon + 0x22);
        } else {
            Draw_AllocSprite(icon + 0x36);
        }
        Draw_OffsetCursor(0x24, 0);
    } else {
        Draw_PrintTextById(0x66);
        Draw_OffsetCursor(Draw_MeasureTextWidth(Text_LookupTable4(0x66)), 0);
    }

    Draw_PrintTextById((g_MenuSelectedItemIndex & 1) | 0x64);
    Draw_StatePop();

    if ((g_MenuSelectedItemIndex & 1) == 0) {
        Draw_OffsetCursor(0, 0xE);
        Menu_DrawActiveListSlot(selected);
        Draw_OffsetCursor(0, 0xE);
        Draw_PrintTextById(0x67);
    }

    Draw_OffsetCursor(0, 0x14);
    Draw_PrintTextById(0x68);
}

int Menu_StepSkillSelect(MenuWidgetNode *arg0, int arg1) {
    M2C_UNK var_a0;
    s32 *temp_a1;
    s32 pad_[1];
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    s32 var_v0;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (temp_v0 != 0) {
            if (temp_v0 == 1) {
                MenuWidget_DestroyNode(arg0);
                Menu_PlayCancelSound();
                return 1;
            }
            return 1;
        }
        {
            MenuWidget_DestroyNode(arg0);
            Inv_TransferItemBetweenLists(g_MenuSelectedItemList, g_MenuSelectedItemSlot, g_MenuSelectedItemIndex, g_StatBaseTable[g_MenuSelectedItemIndex]);
            temp_a1 = &g_StatBaseTable[g_MenuSelectedItemIndex];
            temp_v0_2 = *temp_a1;
            *temp_a1 = temp_v0_2 - (temp_v0_2 < 0x3E7);
            MenuWidget_NavScrollTo(0x2C);
            MenuWidget_NavScrollTo(0xB);
            MenuWidget_NavScrollTo(0xA);
            MenuWidget_NavScrollTo(5);
            MenuWidget_NavScrollTo(6);
            MenuWidget_NavScrollTo(7);
            MenuWidget_NavScrollTo(0x1C);
            var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0);
            if (var_v0 == 0) {
                var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
            }
            MenuWidget_SetCurrentNode(var_v0);
            var_s1 = 0;
            Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
            do {
                temp_s0 = Inv_RestoreSelection(var_s1);
                if ((Inv_IsActiveListOverrideSelected() == 0) && ((var_a0 = 2, (temp_s0 == g_InventoryRuntime.tracked[0])) || (var_a0 = 3, (temp_s0 == g_InventoryRuntime.tracked[2])))) {
                    Inv_SetActiveList(var_a0, 0);
                }
                var_s1 += 1;
            } while (var_s1 < 2);
            Menu_PlayConfirmSound();
            return 1;
        }
    } else {
        if (arg1 & 0x40) {
shared_1:
            MenuWidget_DestroyNode(arg0);
            Menu_PlayCancelSound();
        }
        return 1;
    }
}

void Menu_CreateEquipStatsPanel(int arg0) {
    MenuWidgetNode *node;
    MenuWidgetNode *child;

    node = MenuWidget_CreateSimpleNode(0xA, arg0, 0, 0);
    node->draw = Menu_DrawEquipStats;
    node->update = Menu_EquipStatsInputHandler;

    child = MenuWidget_CreateNode(0x1C, node, node);
    child->draw = Menu_DrawItemListInvPanel;
    child->cursor_x = -1;
}

void Menu_DrawEquipStats(void) {
    int value;
    ItemDataRecord *data;

    value = Inv_RestoreSelection(1);
    data = Inv_LookupActiveListData(value);

    Draw_OffsetCursor(4, 4);
    Menu_DrawActiveListSlot(value);
    Draw_OffsetCursor(-4, -4);
    Menu_DrawEquipStatsDelta(data);

    if (data != 0) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(data->baseStats[2]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->bonusStats[2]);

        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(data->baseStats[1]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->bonusStats[1]);

        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(data->baseStats[0]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->bonusStats[0]);

        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }
}

int Menu_EquipStatsInputHandler(MenuWidgetNode *node, int flags) {
    MenuWidgetNode *selected;
    MenuWidgetNode *child;
    void *target;

    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x4000) {
        child->cursor_x = -1;
        child = MenuWidget_FindByModeAndSelectedBase(2, 0xB);
        selected = child;
        selected->cursor_x = 0;
        selected->cursor_y = 0;
        MenuWidget_SetCurrentNode(selected);
        Menu_PlayMoveSound();
        return 1;
    }
    if (flags & 0x10000) {
        Menu_OpenItemList();
        return 1;
    }
    if (flags & 0x40) {
        Inv_BuildStorageDisplay();
        MenuWidget_NavScrollTo(0x2C);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xA);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_NavScrollTo(7);
        MenuWidget_NavScrollTo(0x1C);
        target = MenuWidget_FindByModeAndSelectedBase(2, 0);
        if (target == NULL) {
            target = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
        }
        MenuWidget_SetCurrentNode(target);
        Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_CreateItemDetailView(MenuWidgetNode *arg0) {
    MenuWidgetNode *node;
    MenuWidgetNode *parent;
    MenuWidgetNode *link;

    parent = MenuWidget_CreateSimpleNode(0xB, arg0, 0, 0);
    node = MenuWidget_CreateNode(0xB, parent, parent);
    parent->update = Menu_DetailViewInput;
    node->draw = Menu_DrawUsableItemActionList2;

    Inv_SelectActiveList(0);
    Draw_SetPrimCallback(node, ((char *)Inv_LookupActiveListData(Inv_RestoreSelection(1)))[0x14]);
    MenuWidget_ClearColumnLayout(node);

    link = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (link != 0) {
        node->linkedPrevious = link;
        link->linkedNext = node;
    }

    Inv_InitWayneStorage();
}

int Menu_DetailViewInput(MenuWidgetNode *arg0, unsigned int arg1) {
    MenuWidgetNode *node;

    node = MenuWidget_GetChild(arg0, 0);
    Menu_InventoryNavigate(node, MenuWidget_FindByModeAndSelectedBase(2, 6), arg1);

    if (arg1 & 0x1000) {
        node->cursor_x = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x1C);
        node->cursor_x = 0;
        node->cursor_y = node->y_limit - 1;
        MenuWidget_SetCurrentNode(node);
        Menu_PlayMoveSound();
    }

    return 1;
}

int Menu_InventoryNavigate(MenuWidgetNode *arg0, MenuWidgetNode *arg1, unsigned int flags) {
    int ret;
    MenuWidgetNode *node;

    ret = 0;
    if (flags & 0x10000) {
        Menu_OpenItemList();
        ret = 1;
    } else if (flags & 0x40) {
        Inv_BuildStorageDisplay();
        MenuWidget_NavScrollTo(0x2C);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xA);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_NavScrollTo(7);
        MenuWidget_NavScrollTo(0x1C);

        node = MenuWidget_FindByModeAndSelectedBase(2, 0);
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
        }
        MenuWidget_SetCurrentNode(node);
        Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
        Menu_PlayCancelSound();
        ret = 1;
    }

    return ret;
}

void Menu_CreateItemList(void) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
    ItemDataRecord *data;
    int i;
    int value;

    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->draw = Menu_DrawItemListHeader;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->draw = Menu_DrawNotificationDialogContent;
    MenuWidget_SetCurrentNode(child);

    Menu_SetDeferredCallback(0);

    parent->grid_width = 0xAA;
    parent->x = 0x4B;
    parent->visible_rows += 0xA;

    child->x = parent->grid_width - 0x44;
    child->y += 8;

    g_MenuItemListHeaderGlyph = 0x37;

    if (g_InvTrackedSlots[2] >= 0) {
        data = Inv_LookupActiveListData(g_InvTrackedSlots[2]);

        for (i = 0; i < data->tailCount; i++) {
            if ((data->tailData[i] & 0xE0) == 0xA0) {
                break;
            }
        }

        value = g_MenuItemListHeaderGlyph - 1;
        g_MenuItemListHeaderGlyph = value + (data->tailData[i] & 0x1F);
    }
}

void Menu_DrawItemListHeader(void) {
    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(g_MenuItemListHeaderGlyph);
    Draw_OffsetCursor(0x20, 0);
    Draw_PrintTextById(0x1C);
    Draw_OffsetCursor(-0x20, 0x10);
    Draw_PrintTextById(0x1D);
}

void Menu_OpenItemList(void)
{
    int list = 0;
    int slot = -1;
    int status;
    int kind;
    int i;
    MenuWidgetNode *node;
    ItemDataRecord *item;

    node = MenuWidget_GetCurrentNode();
    switch (node->selected_base) {
    case 6:
        slot = MenuWidget_GridCellIndex(MenuWidget_GetCurrentNode());
    case 27:
        list = 0;
        break;
    case 11:
        slot = MenuWidget_GridCellIndex(MenuWidget_GetCurrentNode());
        list = 1;
        break;
    case 28:
        list = 1;
        break;
    }

    item = Inv_LookupActiveListData(Inv_RestoreSelection(list));
    /* slot == -1 reads tailCount, the byte just before tailData. */
    if (item->tailData[slot]) {

    g_MenuSelectedItemList = list;
    g_MenuSelectedItemSlot = slot;
    status = Inv_CheckItemEquippable(list, slot);
    if (status == 1 || status == 2) {
        kind = Inv_GetActiveListItemType(Inv_RestoreSelection(list));
        g_InvItemUsableFlag = kind != 9;
        kind = (g_StatBaseTable[0] > 0) + (g_StatBaseTable[1] > 0) + (g_StatBaseTable[2] > 0);
        if (g_StatBaseTable[3] > 0 ? kind + 1 >= 2 : kind >= 2) {
            Menu_CreateScreenModeSubmenu();
        } else {
            for (i = 0; i < 4 && g_StatBaseTable[i] == 0; ++i) {}
            g_MenuSelectedItemIndex = i < 4 ? i : 0;
            if (status == 1 || (!(g_MenuSelectedItemIndex & 1) && status == 2)) {
                Menu_CreateItemActionSubmenu();
            } else {
                Menu_CreateItemList();
            }
        }
        Menu_PlayConfirmSound();
        return;
    }
    switch (status) {
    case 0: Menu_CreateItemList(); return;
    case 4: kind = 0x63; break;
    default: kind = 7; break;
    }
    Menu_CreateNotificationDialog(kind, 0);
    return;
    }
    Menu_PlayErrorSound();
}
#undef NULL

void Menu_StepSaveLoadScreen(void)
{
    MenuWidgetNode *container;
    MenuWidgetNode *parent;
    MenuWidgetItemAction transfer;
    MenuWidgetNode *list;
    int slot;

    parent = MenuWidget_CreateSimpleNode(12, 0, 0, 0);
    container = MenuWidget_CreateNode(12, parent, parent);
    parent->update = Menu_SaveLoadNavigate;
    container->draw = Menu_DrawArmorListContainer;
    MenuWidget_SetCurrentNode(container);

    parent = MenuWidget_CreateSimpleNode(13, container, 0, 0);
    container = MenuWidget_CreateNode(13, parent, parent);
    parent->update = Menu_InventoryInputHandler;
    parent->disabled = 1;
    container->draw = Menu_DrawArmorSelectionList;
    transfer = Inv_TransferItem;
    container->itemAction = transfer;
    MenuWidget_ClearColumnLayout(container);
    container->layout_flags &= ~4;
    Draw_SetPrimCallback(container, Menu_GetBattleCount());

    parent = MenuWidget_CreateSimpleNode(14, container, 0, 0);
    list = MenuWidget_CreateNode(14, parent, parent);
    parent->update = Menu_InventoryInputHandler;
    list->draw = Menu_DrawSelectableEquipSlotList;
    list->itemAction = transfer;
    list->refreshItems = Menu_RebuildSelectableMask;
    list->cursor_x = -1;
    g_InvSwapTargetIndex = -1;
    g_InvSelectedItemIndex = -1;
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
    g_MenuActionSubmenuOpen = 0;
}

int Menu_SaveLoadNavigate(MenuWidgetNode *arg0, unsigned int flags) {
    MenuWidgetNode *node;
    int mode;

    if (flags & 0x10000) {
        node = MenuWidget_GetChild(arg0, 0);
        mode = MenuWidget_GridCellIndex(node);

        switch (mode) {
            case 0:
                Inv_TransferItemAlt();
                node->cursor_y = 2;
                break;

            case 1:
                node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
                node->cursor_y = 0;
                node->cursor_x = 0;
                MenuWidget_SetCurrentNode(node);
                break;

            case 2:
                Inv_ClearDisplaySlots();
                MenuWidget_InitPool();
                Inv_SetActiveList(0xA, 0);
                break;

            default:
                return 1;
        }

        Menu_PlayConfirmSound();
    }

    return 1;
}

/* The inventory root step, equip panel alignment, the bonus-point slot
 * handlers and the bonus-point allocation flow with its item list input
 * (main_tu_evidence G0426). */

/* Defined here so the assembler sees a small-data symbol: retail waits a
 * load-delay slot before the gp-relative store of the reserve ammo. */
int g_MenuStatItemAmount;

/* Builds the inventory screen widget tree: the optional battle command list,
 * the item list for `list` (preselecting `item` when it is present), the bonus
 * point and parasite ability panels, and the item action list. */
void Menu_StepInventoryRoot(int mode, int list, int item)
{
    ItemDataRecord *data;
    MenuWidgetNode *panel;
    MenuWidgetNode *node;
    int i;
    int slot;

    data = Inv_LookupActiveListData(list);
    if (g_MenuEquipMode) {
        panel = MenuWidget_CreateSimpleNode(0x36, MenuWidget_GetCurrentNode(), 0, 0);
        node = MenuWidget_CreateNode(0x36, panel, panel);
        panel->update = Menu_InventoryPageNavHandler;
        if (g_MenuItemUseMode)
            MenuWidget_OffsetPosition(panel, 0, 0x14);
        node->draw = Menu_DrawBattleCommandList;
        MenuWidget_SetCurrentNode(node);
    }
    panel = MenuWidget_CreateSimpleNode(0xD, MenuWidget_GetCurrentNode(), 0, 0);
    node = MenuWidget_CreateNode(g_MenuItemUseMode ? 0x10 : 0xD, panel, panel);
    panel->update = Menu_ItemListInputHandler;
    panel->disabled = 1;
    node->draw = Menu_DrawEquipSelectionList;
    if (g_MenuLayoutLocked) {
        MenuWidget_SetColumnLayout(node, 0x15);
        if (node->cursor_x < 0)
            node->cursor_x = 0;
    }
    if (g_MenuEquipMode) {
        MenuWidget_OffsetPosition(panel, 0, g_MenuItemUseMode ? -0x10 : 0x14);
        node->cursor_x = -1;
    }
    g_MenuSourceItemList = Inv_IsActiveListOverrideSelected();
    Inv_SelectActiveList(0);
    g_MenuSourceItemSlot = list;
    g_MenuItemRenameMode = list == -2;
    g_MenuItemDiscardMode = list == -3;
    if (g_MenuItemRenameMode) {
        MenuWidget_ClearColumnLayout(node);
        node->cursor_x = 0;
    }
    if (g_MenuLayoutLocked)
        Inv_BuildFilteredPackedListExcluding(mode, -1);
    else if (!g_MenuItemRenameMode && !g_MenuItemDiscardMode)
        Inv_BuildFilteredPackedList(mode);
    Draw_SetPrimCallback(node, Inv_GetPackedListCount());
    if (item >= 0) {
        for (i = 0; i < Inv_GetPackedListCount(); i++) {
            if (Inv_GetPackedListItem(i) == item)
                break;
        }
        if (i < Inv_GetPackedListCount()) {
            node->cursor_y = i;
            MenuWidget_ClampScroll(node);
        }
    }
    if (node->cursor_x >= 0 && !g_MenuEquipMode)
        MenuWidget_SetCurrentNode(node);
    if (g_MenuItemUseMode) {
        MenuWidget_OffsetPosition(panel, 0, 0x38);
        panel->visible_rows -= 0x30;
        panel = MenuWidget_CreateSimpleNode(0x18, 0, 0, 0);
        panel->draw = Menu_DrawBonusPointSlotValue;
        panel->grid_width += 0x14;
        MenuWidget_OffsetPosition(panel, -0x88, 0);
        if (!g_MenuEquipMode) {
            int unfocused = node->cursor_x < 0;

            panel = MenuWidget_CreateSimpleNode(0x30,
                MenuWidget_FindByModeAndSelectedBase(2, 0), 0, 0);
            node = MenuWidget_CreateNode(0x30, panel, panel);
            panel->update = Menu_StatSlotInputHandler;
            node->draw = Menu_DrawParasiteAbilityList;
            if (unfocused) {
                node->cursor_y = 0;
                node->cursor_x = 0;
            }
            if (node->cursor_x >= 0)
                MenuWidget_SetCurrentNode(node);
        }
        {
            u16 *source = g_AyaSaveState.stats.levels;
            int stat;

            for (stat = 0; stat < 7; stat++) {
                g_BonusPointStatDeltas[stat] = *source++;
                g_BonusPointStatMultipliers[stat] = 0;
                Aya_StatQueryLevelAndSubLevel(stat, g_BonusPointStatDeltas[stat], &g_BonusPointStatQueryResults[stat], 0);
            }
        }
        g_MenuBonusPointBarAnimActive = 0;
        g_BonusPointBarAnimProgress = 0;
        g_BonusPointDisplayValue = g_AyaSaveState.bonus_points;
        for (slot = 6; slot >= 0; slot--)
            g_BonusPointStatClearedWordsWriteOnly[slot] = 0;
        Menu_InitStateTables();
    }
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    if (g_MenuLayoutLocked && !MenuWidget_FindByModeAndSelectedBase(1, 0x2F))
        MenuWidget_CreateSimpleNode(0x2F, 0, 0, 0)->draw = Menu_DrawItemDetailPanel;
    panel = MenuWidget_CreateSimpleNode(0xB, node, 0, 0);
    node = MenuWidget_CreateNode(0xB, panel, panel);
    panel->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    MenuWidget_ClearColumnLayout(node);
    MenuWidget_OffsetPosition(panel, 6, 0);
    panel = MenuWidget_CreateSimpleNode(0xF, node, 0, 0);
    if (g_MenuItemUseMode) {
        node = MenuWidget_CreateNode(0x1D, panel, panel);
        panel->update = Menu_BonusPointCancelHandler;
        node->draw = Menu_DrawEmptyList;
        node->cursor_x = -1;
        g_MenuItemUseSelectedItemSnapshot.iconId = 0;
    }
    panel->draw = Menu_StepEquipSlotSelect2;
    if (data)
        g_MenuStatItemStatIndex = (u8)data->bonusStats[0] & 3;
    else
        g_MenuStatItemStatIndex = 0;
    if (data)
        g_MenuStatItemAmount = data->reserveAmmo;
    else
        g_MenuStatItemAmount = 0;
    g_InvItemUsableFlag = mode != 0x200;
    panel = MenuWidget_GetCurrentNode();
    if (panel->selected_base == 0x30 || panel->selected_base == 0x36)
        Menu_SetEquipPanelsCursorY();
    else
        Menu_AlignEquipPanels();
}

void Menu_AlignEquipPanels(void) {
    MenuWidgetNode *panel;
    MenuWidgetNode *child;
    int shift;

    panel = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
    child = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
    shift = (g_MenuItemUseMode != 0 ? 0xB0 : child->popup_node != 0 ? 0xA2 : 0x9C) - panel->x;
    MenuWidget_OffsetPosition(panel, shift, 0);
    MenuWidget_ClearCursorY(panel);
    panel = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    MenuWidget_OffsetPosition(panel, shift, 0);
    MenuWidget_ClearCursorY(panel);
    panel = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
    MenuWidget_OffsetPosition(panel, shift, 0);
    MenuWidget_ClearCursorY(panel);
}

void Menu_SetEquipPanelsCursorY(void) {
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xF));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xB));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x2F));
}

int g_MenuItemUseMode;
int g_BonusPointDisplayValue;

void Menu_DrawBonusPointSlotValue(void) {
    int value;

    Draw_OffsetCursor(6, 4);
    Draw_AllocSprite(0x93);
    value = g_MenuItemUseMode;
    Draw_OffsetCursor((value * 5 * 4) + 0x48, 0);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
}

void Menu_ExitBonusPointAllocation(void) {
    s32 index;
    u16 *dst;
    s32 *src;
    u16 value;

    dst = g_AyaStatAgility;
    index = 0;
    src = g_BonusPointStatDeltas;
    do {
        value = *src;
        src++;
        index += 1;
        *dst = value;
        dst++;
    } while (index < 7);
    g_AyaBonusPoints[0] = g_BonusPointDisplayValue;
    Inv_RecalcSlotStats();
    MenuWidget_NavScrollTo(0xF);
    MenuWidget_NavScrollTo(0xB);
    MenuWidget_NavScrollTo(0xD);
    MenuWidget_NavScrollTo(0x18);
    MenuWidget_NavScrollTo(0x30);
    if (g_MenuEquipMode != 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
    } else {
        if (MenuWidget_GetCurrentNode() != 0) {
            MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
        }
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    }
    g_MenuItemRenameMode = 0;
    g_MenuItemUseMode = 0;
    if (g_MenuEquipMode == 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
        Menu_CreateBonusPointAllocationView();
        g_MenuBattleStatusOverlayActive = 1;
    }
}

int g_MenuItemUseMode;

int Menu_StatSlotInputHandler(MenuWidgetNode *node, unsigned int flags) {
    MenuWidgetNode *current = MenuWidget_GetChild(node, 0);

    if (flags & 0x4000) {
        MenuWidgetNode *panel;
        MenuWidgetNode *other;
        int x, offset;

        current->cursor_x = -1;
        current = MenuWidget_FindByModeAndSelectedBase(2, 16);
        current->cursor_x = 0;
        current->cursor_y = 0;
        MenuWidget_SetCurrentNode(current);
        panel = MenuWidget_FindByModeAndSelectedBase(1, 15);
        other = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 13), 0);
        x = panel->x;
        offset = (g_MenuItemUseMode ? 176 : other->popup_node ? 162 : 156) - x;
        MenuWidget_OffsetPosition(panel, offset, 0);
        MenuWidget_ClearCursorY(panel);
        panel = MenuWidget_FindByModeAndSelectedBase(1, 11);
        MenuWidget_OffsetPosition(panel, offset, 0);
        MenuWidget_ClearCursorY(panel);
        panel = MenuWidget_FindByModeAndSelectedBase(1, 47);
        MenuWidget_OffsetPosition(panel, offset, 0);
        MenuWidget_ClearCursorY(panel);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        Menu_OpenBonusPointSpendDialog(current, MenuWidget_GridCellIndex(current) + 5);
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        Menu_ExitBonusPointAllocation();
        Menu_PlayCancelSound();
    }
    return 1;
}

#define NULL ((void *)0)

s32 Menu_BonusPointCancelHandler(void *arg0, s32 arg1) {
    s32 temp_s0;
    s32 var_a0;
    u16 *var_a1;
    s32 *var_v1;
    u16 temp_v0;

    temp_s0 = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_OpenBonusPointSpendDialog(temp_s0, MenuWidget_GridCellIndex(temp_s0));
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_NavScrollTo(0xF);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xD);
        MenuWidget_NavScrollTo(0x18);
        MenuWidget_NavScrollTo(0x30);
        if (g_MenuEquipMode != 0) {
            MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
        } else {
            if (MenuWidget_GetCurrentNode() != NULL) {
                MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
            }
            MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        }
        g_MenuItemRenameMode = 0;
        var_a1 = g_AyaStatAgility;
        var_a0 = 0;
        var_v1 = g_BonusPointStatDeltas;
        do {
            temp_v0 = *var_v1;
            var_v1++;
            var_a0 += 1;
            *var_a1 = temp_v0;
            var_a1 += 1;
        } while (var_a0 < 7);
        g_AyaBonusPoints[0] = g_BonusPointDisplayValue;
        Menu_StepInventoryRoot(0x37E, -1, -1);
        Menu_PlayCancelSound();
    }
    return 1;
}

/* Leaves the item list: scrolls the inventory panels back and returns the
 * focus to the equipment slots or the category tabs. */
static inline void Menu_CloseItemList(void)
{
    MenuWidget_NavScrollTo(0xF);
    MenuWidget_NavScrollTo(0xB);
    MenuWidget_NavScrollTo(0xD);
    MenuWidget_NavScrollTo(0x18);
    MenuWidget_NavScrollTo(0x30);
    if (g_MenuEquipMode != 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
    } else {
        if (MenuWidget_GetCurrentNode() != 0)
            MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    }
    g_MenuItemRenameMode = 0;
}

static inline void Menu_ResetSlotCursors(void)
{
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xF));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xB));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x2F));
}

/* Item action list opened next to the inventory panel. */
static inline void Menu_CreateUsableItemActionView(MenuWidgetNode *parent)
{
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(6, parent, 0, 0);
    node = MenuWidget_CreateNode(6, root, root);
    root->update = Menu_InventoryPageInputHandler;
    root->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    node->layout_flags |= 0x80;
    if (g_MenuLayoutLocked != 0)
        MenuWidget_ClearColumnLayout(node);
    else if (g_InvItemUsableFlag == 0)
        MenuWidget_SetColumnLayout(node, 0x14);
    if (node->cursor_x >= 0)
        MenuWidget_SetCurrentNode(node);
}

/* Same panel as Menu_CreateEquipItemSelectionView. */
static inline void Menu_CreateEquipItemSelectionView(MenuWidgetNode *parent)
{
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(5, parent, 0, 0);
    node = MenuWidget_CreateNode(5, root, root);
    root->draw = Menu_StepEquipSlotSelect;
    root->update = Menu_StepSkillScreen;
    node->draw = Menu_DrawSoundTestList;
    if (g_MenuLayoutLocked != 0)
        MenuWidget_ClearColumnLayout(node);
    else if (g_InvItemUsableFlag == 0)
        MenuWidget_SetColumnLayout(node, 0x12);
    if (node->cursor_x >= 0)
        MenuWidget_SetCurrentNode(node);
    root->disabled = 1;
    node = MenuWidget_CreateNode(0x1B, root, root);
    node->draw = Menu_DrawItemListInvPanel;
    MenuWidget_ClearColumnLayout(node);
}

/* Whether the selected item can go into an equipment slot that has room. */
static inline int Menu_CanEquipSelection(int selection)
{
    int usable;

    usable = 0;
    if (g_StatBaseTable[0] + g_StatBaseTable[1] + g_StatBaseTable[2] + g_StatBaseTable[3] != 0
        && !(Inv_LookupActiveListData(selection)->flags & 0x40)) {
        if (Inv_SetupSlotDisplay(Inv_GetActiveListItemType(selection) >= 6
                                     ? 1 << Inv_GetActiveListItemType(selection)
                                     : 0x3E) != 0)
            usable = 1;
    }
    return usable;
}

/* Input handler of the inventory item list: up returns to the equipment
 * slots, confirm applies a pending stat bonus, opens the equip, examine,
 * rename or use flows, and cancel backs out of the active mode. */
int Menu_ItemListInputHandler(MenuWidgetNode *node, unsigned int flags)
{
    ItemDataRecord *data;
    MenuWidgetNode *child;
    MenuWidgetNode *found;
    MenuWidgetNode *target;
    MenuWidgetNode *panel;
    MenuWidgetNode *list;
    int index;
    int item;
    int selection;
    int usable;
    int mask;
    int kind;
    int value;

    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x1000) {

        found = MenuWidget_FindByModeAndSelectedBase(2, 0x30);
        if (found != 0) {
            child->cursor_x = -1;
            found->cursor_x = 0;
            found->cursor_y = 1;
            MenuWidget_SetCurrentNode(found);
            Menu_ResetSlotCursors();
            Menu_PlayMoveSound();
        }
        found = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
        if (found != 0) {
            child->cursor_x = -1;
            MenuWidget_SetCurrentNode(found);
            Menu_ResetSlotCursors();
            Menu_PlayMoveSound();
            return 1;
        }
    } else if (flags & 0x10000) {
        index = MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 0xD));
        item = Inv_GetPackedListItem(index);
        data = Inv_LookupActiveListData(item);
        if (g_MenuSourceItemSlot >= 0) {
            switch (g_MenuStatItemStatIndex) {
            case 0:
                data->bonusStats[0] = data->bonusStats[0] + g_MenuStatItemAmount >= 1000
                                          ? 999 : data->bonusStats[0] + g_MenuStatItemAmount;
                break;
            case 1:
                data->bonusStats[1] = data->bonusStats[1] + g_MenuStatItemAmount >= 1000
                                          ? 999 : data->bonusStats[1] + g_MenuStatItemAmount;
                break;
            case 2:
                data->bonusStats[2] = data->bonusStats[2] + g_MenuStatItemAmount >= 1000
                                          ? 999 : data->bonusStats[2] + g_MenuStatItemAmount;
                break;
            }
            Inv_SelectActiveList(g_MenuSourceItemList);
            Inv_RemoveActiveListItem(g_MenuSourceItemSlot);
            Menu_CloseItemList();
            if (g_MenuEquipMode == 1) {
                MenuWidget_NavScrollTo(0x36);
                Menu_ReopenEquipScreen();
            }
            Menu_PlayConfirmSound();
            return 1;
        } else if (g_MenuLayoutLocked != 0) {
            if (Menu_CanEquipSelection(Inv_RestoreSelection(0)) != 0) {
                Menu_CloseItemList();
                MenuWidget_OffsetPosition(MenuWidget_FindByModeAndSelectedBase(1, 0x36), 0x98, 0);
                g_InvItemUsableFlag = data->kind != 9;
                panel = MenuWidget_FindByModeAndSelectedBase(2, 0);
                if (panel == 0)
                    panel = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
                Menu_CreateUsableItemActionView(panel);
                Menu_CreateEquipItemSelectionView(panel);
                Menu_StepSkillList(panel, 1);
                if (g_MenuEquipMode == 0) {
                    MenuWidgetNode *submenu;

                    submenu = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                    MenuWidget_CreateNode(0x35, submenu, submenu)->draw = Menu_SetupSkillSubmenu;
                }
                /* The list child is no longer needed: reuse it for the equipment node. */
                child = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
                if (child != 0)
                    MenuWidget_SetCurrentNode(child);
                return 1;
            }
        } else if (g_MenuItemUseMode != 0) {
            MenuWidget_NavScrollTo(0xD);
            MenuWidget_NavScrollTo(0x30);
            MenuWidget_NavScrollTo(0x36);
            target = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
            target->parent = MenuWidget_FindByModeAndSelectedBase(2, 0);
            if (target->parent == 0)
                target->parent = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
            MenuWidget_OffsetPosition(target, 0x1C - target->x, 0x14);
            target = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
            MenuWidget_OffsetPosition(target, 0x1C - target->x, 0x14);
            target = MenuWidget_FindByModeAndSelectedBase(2, 0x1D);
            target->cursor_x = 0;
            target->cursor_y = 0;
            kind = Inv_GetActiveListItemType(Inv_RestoreSelection(0));
            target->y_limit = kind == 8 ? 1 : kind == 6 ? 2 : 3;
            MenuWidget_SetCurrentNode(target);
            g_MenuItemUseSelectedItemSnapshot = *Inv_LookupActiveListData(Inv_RestoreSelection(0));
            Menu_PlayConfirmSound();
            return 1;
        } else if (g_MenuItemRenameMode != 0 && Inv_TestSelectionBit(index) != 0) {
            Menu_CloseItemList();
            Menu_OpenRenameScreen(item);
            Menu_PlayConfirmSound();
            return 1;
        } else if (g_MenuItemDiscardMode != 0) {
            if (Inv_TestSelectionBit(index) != 0) {
                MenuWidgetNode *dialog;
                MenuWidgetNode *options;
                MenuConfirmCallback action;
                int width;

                dialog = MenuWidget_CreateSimpleNode(0x29, child, 0, 1);
                options = MenuWidget_CreateNode(0x29, dialog, dialog);
                dialog->draw = Menu_DrawItemLabel;
                dialog->update = Menu_ConfirmDialogHandler;
                options->draw = Menu_DrawActionOptionList;
                g_MenuActionTextBase = 5;
                MenuWidget_SetCurrentNode(options);
                Inv_SelectActiveList(g_MenuActiveItemList);
                g_MenuItemLabelBuffer[0] = 0xFF;
                Text_AppendFFTerminatedBytes(g_MenuItemLabelBuffer, Text_LookupTable4(0x61));
                g_MenuItemPrimListHandles[0] = 0;
                action = Menu_ItemUseAction;
                if (Draw_MeasureTextWidth(g_MenuItemLabelBuffer) < 0x78)
                    width = 0x78;
                else
                    width = Draw_MeasureTextWidth(g_MenuItemLabelBuffer);
                dialog->grid_width = width + 0x14;
                dialog->visible_rows = 0x32;
                dialog->x = (300 - width) >> 1;
                options->x = (dialog->grid_width - 0x80) >> 1;
                g_MenuConfirmCallback = action;
                options->y = dialog->visible_rows - 0x14;
                Menu_PlayConfirmSound();
                return 1;
            }
            Menu_CreateNotificationDialog(0x62, 0);
        }
        Menu_PlayErrorSound();
        return 1;
    } else if (flags & 0x40) {
        found = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
        if (found != 0) {
            child->cursor_x = -1;
            child->scroll_y = 0;
            MenuWidget_SetCurrentNode(found);
            Menu_ResetSlotCursors();
        } else if (g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) {
            g_MenuItemRenameMode = 0;
            g_MenuItemDiscardMode = 0;
            Inv_SetActiveList(9, 0);
        } else {
            MenuWidget_NavScrollTo(0x36);
            if (g_MenuLayoutLocked != 0) {
                g_MenuLayoutLocked = 0;
                Menu_CloseItemList();
                MenuWidget_NavScrollTo(0x2F);
                if (g_MenuEquipMode == 0) {
                    Menu_CreateBonusPointAllocationView();
                    g_MenuBattleStatusOverlayActive = 1;
                }
            } else if (g_MenuItemUseMode != 0) {
                Menu_ExitBonusPointAllocation();
            } else {
                Menu_CloseItemList();
            }
        }
        Menu_PlayCancelSound();
    }
    return 1;
}
#undef g_BonusPointStatDeltas
#define CLAMP_EQUIP_PREVIEW(out, item, stat_index, active_stat) { \
    int desired_ = (active_stat); \
    int current_ = g_MenuStatItemStatIndex; \
    int raw_ = (item)->bonusStats[(stat_index)]; \
    if ((current_ == desired_ && raw_ + g_MenuStatItemAmount < 1000) || \
        (current_ != desired_ && raw_ < 1000)) { \
        (out) = (item)->bonusStats[(stat_index)]; \
        if (g_MenuStatItemStatIndex == (active_stat)) { \
            (out) += g_MenuStatItemAmount; \
        } \
    } else { \
        (out) = 999; \
    } \
}

void Menu_StepEquipSlotSelect2(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *cursor_node;
    ItemDataRecord *item;
    int slot;
    int value;

    node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    if (node != 0) {
        if (node->cursor_x >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (g_MenuItemUseMode != 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        if (node != 0 && node->cursor_x >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (g_MenuLayoutLocked == 0) {
        if (g_InvItemUsableFlag != 0) {
            Inv_RememberSelection(0, g_InventoryRuntime.tracked[0]);
        } else {
            Inv_RememberSelection(0, g_InventoryRuntime.tracked[2]);
        }
    }

    slot = Inv_RestoreSelection(0);
    Draw_OffsetCursor(4, 4);
    Menu_DrawActiveListSlot(slot);
    Draw_OffsetCursor(-4, -4);

    item = Inv_LookupActiveListData(slot);
    g_InvItemUsableFlag = (item->kind != 9);
    Menu_DrawEquipStatsDelta(item);

    if ((g_MenuLayoutLocked != 0 || g_MenuItemUseMode != 0 || g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) &&
        item != 0) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(item->baseStats[2]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(item->baseStats[1]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(item->baseStats[0]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }

    if (g_MenuSourceItemSlot >= 0) {
        Draw_OffsetCursor(0x28, -0x32);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);
        Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(item->bonusStats[2]);

        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0xA, -0xA);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);

        CLAMP_EQUIP_PREVIEW(value, item, 0, 0);
        Draw_SetStatCompareColor(item->bonusStats[0], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 1, 1);
        Draw_SetStatCompareColor(item->bonusStats[1], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 2, 2);
        Draw_SetStatCompareColor(item->bonusStats[2], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_SetColor(0x808080);
    }

    cursor_node = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    {
        void (*set_cursor_y)(MenuWidgetNode *node);
        set_cursor_y = item->tailCount != 0 ? MenuWidget_ClearCursorY : MenuWidget_SetCursorY;
        set_cursor_y(cursor_node);
    }
}

void Menu_DrawBonusPointSpendPanel(void);
s32 Menu_SpendBonusPoints(void *node, u32 buttons);

void Menu_OpenBonusPointSpendDialog(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *node;
    InvItemSlot *record;

    node = MenuWidget_CreateSimpleNode(9, arg0, 0, 1);
    node->draw = Menu_DrawBonusPointSpendPanel;
    node->update = Menu_SpendBonusPoints;
    node->flags = 1;
    MenuWidget_SetCurrentNode(node);

    g_BonusPointSpendStat = arg1;
    g_BonusPointSpendWorkingValue = g_BonusPointDisplayValue;
    if (arg1 < 3) {
        record = Inv_LookupActiveListData(Inv_RestoreSelection(0));
        g_BonusPointSpendItemCopy = *record;
        node->grid_width -= 0x14;
        MenuWidget_OffsetPosition(node, 0xA, 0);
    } else {
        g_BonusPointSpendCurrentValue = g_BonusPointStatDeltas[arg1];
    }
    g_MenuSpendArrowDirection = 0;
}

void Menu_DrawBonusPointSpendPanel(void) {
    int value;
    int mode;
    int level;
    int distance;
    int sublevel;

    Draw_OffsetCursor(0x14, 0x15);
    Draw_AllocSprite(g_MenuSpendArrowDirection + 0x4D);
    Draw_OffsetCursor(-0xE, -0x11);
    Draw_AllocSprite(0x93);

    if (g_BonusPointSpendStat < 3) {
        Draw_OffsetCursor(0x48, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x75, 0x1E);
        Draw_AllocSprite(g_InvItemUsableFlag != 0 ? g_BonusPointSpendStat + 0x7C : g_BonusPointSpendStat + 0x7F);
        Draw_OffsetCursor(0x1E, 0);
        mode = g_BonusPointSpendStat;
        switch (mode) {
        case 0: {
            value = g_BonusPointSpendItemCopy.baseStats[0] + g_BonusPointSpendItemCopy.bonusStats[0];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(g_BonusPointSpendItemCopy.baseStats[0]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(g_BonusPointSpendItemCopy.bonusStats[0]);
            break;
        }
        case 1: {
            value = g_BonusPointSpendItemCopy.baseStats[1] + g_BonusPointSpendItemCopy.bonusStats[1];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(g_BonusPointSpendItemCopy.baseStats[1]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(g_BonusPointSpendItemCopy.bonusStats[1]);
            break;
        }
        case 2: {
            value = g_BonusPointSpendItemCopy.baseStats[2] + g_BonusPointSpendItemCopy.bonusStats[2];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(g_BonusPointSpendItemCopy.baseStats[2]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(g_BonusPointSpendItemCopy.bonusStats[2]);
            break;
        }
        }
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    } else {
        Draw_OffsetCursor(0x5C, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x8A, 0x1F);
        Draw_AllocSprite(g_BonusPointSpendStat + 0x8C);
        Draw_OffsetCursor(0x42, -1);
        Aya_StatQueryLevelAndSubLevel(g_BonusPointSpendStat,
                                   g_BonusPointSpendCurrentValue, &level, 0);
        Draw_PrintNumberWidth2(level + 1);
        Aya_StatQueryDistanceAndSubLevel(g_BonusPointSpendStat,
                                      g_BonusPointSpendCurrentValue, &distance, &sublevel);
        Draw_OffsetCursor(2, 0);
        Draw_AllocTexturedRectAlt(distance, sublevel);
    }
}

int Menu_SpendBonusPoints(void *node, unsigned int buttons)
{
    ItemDataRecord *item;
    int amount;
    int changed;

    item = Inv_LookupActiveListData(Inv_RestoreSelection(0));
    if (buttons & 0x4000) {
        g_MenuSpendArrowDirection = 0;
        if (g_BonusPointSpendWorkingValue >= 100) {
            changed = 0;
            switch (g_BonusPointSpendStat) {
            case 0: {
                short *bonus = &g_BonusPointSpendItemCopy.bonusStats[0];
                changed = *bonus < 999;
                *bonus += changed;
                break;
            }
            case 1: {
                short *bonus = &g_BonusPointSpendItemCopy.bonusStats[1];
                changed = *bonus < 999;
                *bonus += changed;
                break;
            }
            case 2: {
                short *bonus = &g_BonusPointSpendItemCopy.bonusStats[2];
                changed = *bonus < 999;
                *bonus += changed;
                break;
            }
            case 5:
            case 6:
                Aya_StatQueryDistanceAndSubLevel(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue, &amount, 0);
                changed = amount > 0;
                g_BonusPointSpendCurrentValue += amount;
                break;
            }
            g_BonusPointSpendWorkingValue -= changed * 100;
            if (changed) {
                Menu_PlayMoveSound();
                return 1;
            }
        }
        Menu_PlayErrorSound();
    } else if (buttons & 0x1000) {
        g_MenuSpendArrowDirection = 1;
        switch (g_BonusPointSpendStat) {
        case 0: {
            short *bonus = &g_BonusPointSpendItemCopy.bonusStats[0];
            if (*bonus > item->bonusStats[0]) {
                --*bonus;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        case 1: {
            short *bonus = &g_BonusPointSpendItemCopy.bonusStats[1];
            if (*bonus > item->bonusStats[1]) {
                --*bonus;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        case 2: {
            short *bonus = &g_BonusPointSpendItemCopy.bonusStats[2];
            if (*bonus > item->bonusStats[2]) {
                --*bonus;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        case 5:
        case 6: {
            int *minimum = &g_BonusPointStatDeltas[g_BonusPointSpendStat];
            int gap;
            int new_current;
            if (*minimum < g_BonusPointSpendCurrentValue) {
                gap = Aya_StatOverflowQuery(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue);
                if (g_BonusPointSpendCurrentValue - gap < *minimum) {
                    new_current = g_BonusPointStatDeltas[g_BonusPointSpendStat];
                } else {
                    new_current = g_BonusPointSpendCurrentValue - Aya_StatOverflowQuery(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue);
                }
                g_BonusPointSpendCurrentValue = new_current;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        }
    } else if (buttons & 0x10000) {
        g_BonusPointDisplayValue = g_BonusPointSpendWorkingValue;
        if (g_BonusPointSpendStat < 3) {
            *item = g_BonusPointSpendItemCopy;
            if (!Inv_IsActiveListOverrideSelected()) {
                if (item == Inv_LookupActiveListData(g_InventoryRuntime.tracked[0])) {
                    Inv_SetActiveList(2, 0);
                } else if (item == Inv_LookupActiveListData(g_AyaEquippedArmorSlot[0])) {
                    Inv_SetActiveList(3, 0);
                }
            }
        } else {
            g_BonusPointStatDeltas[g_BonusPointSpendStat] = g_BonusPointSpendCurrentValue;
        }
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
    } else if (buttons & 0x40) {
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }
    return 1;
}
