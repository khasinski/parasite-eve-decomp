/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvAmmoSpendActiveList;
int g_InvItemUsableFlag;
int g_InvSelectedItemIndex;
int g_InvSwapSourceList;
int g_InvSwapTargetIndex;
int g_MenuEquipMode;
int g_MenuItemUseMode;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;

#define NULL ((void *)0)

int Menu_AmmoSpendInputHandler(s32 arg0, s32 arg1);
void Menu_DrawAmmoSpendPanel(void);

void Menu_CreateAmmoSpendPanel(MenuWidgetNode *parent) {
    MenuWidgetNode *node;
    ItemDataRecord *item;

    node = MenuWidget_CreateSimpleNode(0x3E, parent, 0, 1);
    node->update = Menu_AmmoSpendInputHandler;
    node->draw = Menu_DrawAmmoSpendPanel;
    node->flags = 1;
    MenuWidget_SetCurrentNode(node);
    Inv_SelectActiveList(g_InvAmmoSpendActiveList);
    item = Inv_LookupActiveListData(g_InvSelectedItemIndex);
    if ((item != NULL) && ((item->kind - 0x13) >= 3U)) {
        s32 active = g_InvAmmoSpendActiveList;
        s32 source = g_InvSwapSourceList;
        s32 selected = g_InvSelectedItemIndex;
        s32 target = g_InvSwapTargetIndex;

        active ^= source;
        source ^= active;
        g_InvAmmoSpendActiveList = active;
        active ^= source;

        selected ^= target;
        target ^= selected;
        g_InvSelectedItemIndex = selected;
        selected ^= target;

        g_InvSwapSourceList = source;
        g_InvAmmoSpendActiveList = active;
        g_InvSwapTargetIndex = target;
        g_InvSelectedItemIndex = selected;
    }
    Inv_BuildDisplayFromList(g_InvAmmoSpendActiveList, g_InvSelectedItemIndex, g_InvSwapSourceList, g_InvSwapTargetIndex);
    Inv_ClearSelectionBitset();
    g_MenuSpendArrowDirection = 0;
}

s32 Menu_AmmoSpendInputHandler(s32 arg0, s32 arg1) {
    if (arg1 & 0x1000) {
        if (Inv_SpendAmmo(-MenuInput_GetRepeatStep()) != 0) {
            Menu_PlayErrorSound();
        } else {
            Menu_PlayMoveSound();
        }
        g_MenuSpendArrowDirection = 1;
    } else if (arg1 & 0x4000) {
        if (Inv_SpendAmmo(MenuInput_GetRepeatStep()) != 0) {
            Menu_PlayErrorSound();
        } else {
            Menu_PlayMoveSound();
        }
        g_MenuSpendArrowDirection = 0;
    } else {
        if (arg1 & 0x10000) {
            Inv_StepScrollDisplay();
            g_InvSwapTargetIndex = -1;
            g_InvSelectedItemIndex = -1;
            Inv_RebuildSelectableMask();
            MenuWidget_DestroyNode(arg0);
            Menu_PlayConfirmSound();
        }
    }
    if (arg1 & 0x40) {
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Inv_RebuildSelectableMask();
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}

int g_MenuSpendArrowDirection;

void Menu_DrawAmmoSpendPanel(void) {
    Draw_OffsetCursor(0x3C, 0x12);
    Draw_AllocSprite(g_MenuSpendArrowDirection + 0x4D);
    Draw_OffsetCursor(-0x38, -0xE);
    Menu_DrawWeaponComparisonPanel();
}
#undef NULL

/* Equip item selection view, its stat delta preview, grid scroll and slot
 * step, and the skill screen input handler it installs. */

#define NULL ((void *)0)

void Menu_CreateEquipItemSelectionView(MenuWidgetNode *parent)
{
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(5, parent, 0, 0);
    node = MenuWidget_CreateNode(5, root, root);
    root->draw = Menu_StepEquipSlotSelect;
    root->update = Menu_StepSkillScreen;
    node->draw = Menu_DrawSoundTestList;
    if (g_MenuLayoutLocked != 0) {
        MenuWidget_ClearColumnLayout(node);
    } else if (g_InvItemUsableFlag == 0) {
        MenuWidget_SetColumnLayout(node, 0x12);
    }
    if (node->cursor_x >= 0) {
        MenuWidget_SetCurrentNode(node);
    }
    root->disabled = 1;
    node = MenuWidget_CreateNode(0x1B, root, root);
    node->draw = Menu_DrawItemListInvPanel;
    MenuWidget_ClearColumnLayout(node);
}

void Menu_DrawEquipStatsDelta(ItemDataRecord *preview) {
    int sprite_base;
    int value;
    MenuWidgetNode *node;

    if (preview == 0) {
        return;
    }

    sprite_base = 0x7F;
    if (g_InvItemUsableFlag != 0) {
        sprite_base = 0x7C;
    }

    Draw_OffsetCursor(4, 0x1C);
    Draw_AllocSprite(sprite_base);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 1);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 2);

    if (g_MenuLayoutLocked != 0) {
        node = MenuWidget_GetCurrentNode();
        if (node->selected_base == 7) {
            Draw_OffsetCursor(0x18, 0xE);
            return;
        }
    }

    Draw_OffsetCursor(0x1E, -0x1C);
    value = preview->baseStats[0] + preview->bonusStats[0];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->baseStats[1] + preview->bonusStats[1];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->baseStats[2] + preview->bonusStats[2];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
}

/* The retail display clamps only the upper bound; negative totals survive. */
static inline int clampedStatTotal(ItemDataRecord *item, int index)
{
    int value = item->baseStats[index] + item->bonusStats[index];
    if (value > 999)
        value = 999;
    return value;
}

void Menu_StepItemGridScroll(ItemDataRecord *item, ItemDataRecord *other)
{
    int sprite;
    if (!other)
        return;
    if (item) {
        if (g_MenuLayoutLocked) {
            Draw_OffsetCursor(40, -50);
            Draw_AllocSprite(0x88);
            Draw_OffsetCursor(0, 10);
            Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
            Draw_OffsetCursor(-20, 14);
            Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
            Draw_OffsetCursor(-20, 14);
            Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
            Draw_OffsetCursor(-38, 12);
        }
        Draw_OffsetCursor(38, -12);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -14);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -14);
        Draw_AllocSprite(0x22);
        if (g_MenuLayoutLocked) {
            Draw_OffsetCursor(12, -10);
            Draw_AllocSprite(0x88);
            Draw_SetStatCompareColor(item->bonusStats[0], other->bonusStats[0]);
            Draw_OffsetCursor(0, 10);
            Draw_PrintSignedNumberWidth4(other->bonusStats[0]);
            Draw_OffsetCursor(-20, 14);
            Draw_SetStatCompareColor(item->bonusStats[1], other->bonusStats[1]);
            Draw_PrintSignedNumberWidth4(other->bonusStats[1]);
            Draw_OffsetCursor(-20, 14);
            Draw_SetStatCompareColor(item->bonusStats[2], other->bonusStats[2]);
            Draw_PrintSignedNumberWidth4(other->bonusStats[2]);
        } else {
            Draw_SetStatCompareColor(clampedStatTotal(item, 0), clampedStatTotal(other, 0));
            Draw_OffsetCursor(12, -2);
            Draw_PrintNumberWidth4(clampedStatTotal(other, 0));
            Draw_OffsetCursor(-36, 14);
            Draw_SetStatCompareColor(clampedStatTotal(item, 1), clampedStatTotal(other, 1));
            Draw_PrintNumberWidth4(clampedStatTotal(other, 1));
            Draw_OffsetCursor(-36, 14);
            Draw_SetStatCompareColor(clampedStatTotal(item, 2), clampedStatTotal(other, 2));
            Draw_PrintNumberWidth4(clampedStatTotal(other, 2));
        }
    } else {
        sprite = g_InvItemUsableFlag ? 0x7C : 0x7F;
        Draw_OffsetCursor(4, 23);
        Draw_AllocSprite(sprite);
        Draw_OffsetCursor(0, 14);
        Draw_AllocSprite(sprite + 1);
        Draw_OffsetCursor(0, 14);
        Draw_AllocSprite(sprite + 2);
        Draw_SetColor(0x8080);
        Draw_OffsetCursor(40, -28);
        Draw_SetStatCompareColor(0, other->baseStats[0] + other->bonusStats[0]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 0));
        Draw_OffsetCursor(-36, 14);
        Draw_SetStatCompareColor(0, other->baseStats[1] + other->bonusStats[1]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 1));
        Draw_OffsetCursor(-36, 14);
        Draw_SetStatCompareColor(0, other->baseStats[2] + other->bonusStats[2]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 2));
    }
    Draw_SetColor(0x808080);
}

void Menu_StepEquipSlotSelect(void)
{
    MenuWidgetNode *node, *current;
    ItemDataRecord *item, *other;
    int selection;
    node = MenuWidget_FindByModeAndSelectedBase(2, 13);
    if (node) {
        if (node->cursor_x >= 0)
            Inv_RememberSelection(0, Inv_GetPackedListItem(MenuWidget_GridCellIndex(node)));
    } else if (g_MenuItemUseMode) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 16);
        if (node && node->cursor_x >= 0)
            Inv_RememberSelection(0, Inv_GetPackedListItem(MenuWidget_GridCellIndex(node)));
    } else if (!g_MenuLayoutLocked) {
        Inv_RememberSelection(0, g_InvItemUsableFlag ? g_InventoryRuntime.tracked[0] : g_InventoryRuntime.tracked[2]);
    }
    if (g_MenuLayoutLocked)
        selection = Inv_RestoreSelection(0);
    else
        selection = g_InvItemUsableFlag ? g_InventoryRuntime.tracked[0] : g_InventoryRuntime.tracked[2];
    item = Inv_LookupActiveListData(selection);
    current = MenuWidget_FindByModeAndSelectedBase(2, 7);
    if (current == MenuWidget_GetCurrentNode()) {
        other = Inv_LookupActiveListData(Inv_RestoreSelection(1));
        Menu_DrawEquipStatsDelta(g_MenuLayoutLocked ? other : item);
        Menu_StepItemGridScroll(item, other);
    } else {
        Menu_DrawEquipStatsDelta(item);
        if (item) {
            Draw_OffsetCursor(42, -12);
            Draw_PrintNumberWidth4Unk(item->baseStats[2]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
            Draw_OffsetCursor(-45, -14);
            Draw_PrintNumberWidth4Unk(item->baseStats[1]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
            Draw_OffsetCursor(-45, -14);
            Draw_PrintNumberWidth4Unk(item->baseStats[0]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
            Draw_OffsetCursor(-45, -10);
            Draw_AllocSprite(0x87);
            Draw_OffsetCursor(25, 0);
            Draw_AllocSprite(0x88);
        }
    }
}

s32 Menu_StepSkillScreen(MenuWidgetNode *panel, s32 flags) {
    MenuWidgetNode *node;
    MenuWidgetNode *root;

    node = MenuWidget_GetChild(panel, 0);
    if (flags & 0x10000) {
        if (Inv_GetPackedListCount() != 0) {
            if (g_MenuLayoutLocked != 0) {
                Menu_OpenItemList();
                return 1;
            }
            if (Battle_IsInputAllowedWrapped() != 0) {
                root = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                MenuWidget_CreateNode(0x35, root, root)->draw = Menu_SetupSkillSubmenu;
                node = MenuWidget_FindByModeAndSelectedBase(2, 7);
                node->cursor_x = 0;
                MenuWidget_SetCurrentNode(node);
                Menu_PlayConfirmSound();
                return 1;
            }
        }
        Menu_PlayErrorSound();
        return 1;
    }
    if (flags & 0x40) {
        if (g_MenuLayoutLocked != 0) {
            Menu_InitCategoryTabs();
            return 1;
        }
        MenuWidget_NavScrollTo(7);
        MenuWidget_DestroyNode(panel);
        panel = MenuWidget_FindByModeAndSelectedBase(1, 6);
        if (panel != NULL) {
            MenuWidget_DestroyNode(panel);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1C));
            Menu_CreateBonusPointAllocationView();
            Menu_PlayCancelSound();
        }
        return 1;
    }
    if ((flags & 0x4000) && MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 6)) != 0) {
        node->cursor_x = -1;
        node = MenuWidget_GetChild(panel, 1);
        if (node != NULL) {
            node->cursor_x = -1;
        }
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node != NULL) {
            node->cursor_y = 0;
            node->cursor_x = 0;
            MenuWidget_SetCurrentNode(node);
        }
        Menu_PlayMoveSound();
    }
    return 1;
}

void Menu_CreateInventoryListView(MenuWidgetNode *parent) {
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(6, parent, 0, 0);
    node = MenuWidget_CreateNode(6, root, root);
    root->update = Menu_InventoryPageInputHandler;
    root->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    node->layout_flags |= 0x80;
    if (g_MenuLayoutLocked != 0) {
        MenuWidget_ClearColumnLayout(node);
    } else if (g_InvItemUsableFlag == 0) {
        MenuWidget_SetColumnLayout(node, 0x14);
    }
    if (node->cursor_x >= 0) {
        MenuWidget_SetCurrentNode(node);
    }
}

void Menu_DrawItemDetailPanel(MenuWidgetNode *panel) {
    MenuWidgetNode *selection;
    ItemDataRecord *item;
    int index;
    int base;
    int panel_y;
    /* Match debt: retail computes dx right after its two loads, in the
     * argument registers; unpinned it is scheduled after the tail-count
     * chain (lev 5). */
    register int dx asm("$5");
    register int dy asm("$6");
    int sum;
    int displayed;
    MenuWidgetNode *arg_panel;
    int panel_offset;

    if (!MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 7))) {
        selection = MenuWidget_FindByModeAndSelectedBase(1, 6);
        if (selection == 0) selection = MenuWidget_FindByModeAndSelectedBase(1, 0xB);

        if (MenuWidget_FindByModeAndSelectedBase(1, 0xB) == 0 &&
            MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 7)) >= 0) {
            index = Inv_GetPackedListItem(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 7)));
        } else {
            index = Inv_RestoreSelection(0);
        }

        item = Inv_LookupActiveListData(index);
        arg_panel = panel;
        dx = selection->x - arg_panel->x;
        dy = ((item->tailCount + 1) >> 1) * 16;
        panel_offset = arg_panel->y - 0x6C;
        dy -= panel_offset;
    } else {
        base = g_MenuEquipMode ? 0x38 : 0x24;
        panel_y = panel->y;
        if (Inv_GetPackedListCount() < 9) {
            panel_offset = base + Inv_GetPackedListCount() * 16;
            dy = panel_offset - panel_y;
        } else {
            panel_offset = panel_y - 0x80;
            dy = base - panel_offset;
        }
        arg_panel = panel;
        dx = 0;
    }

    MenuWidget_OffsetPosition(arg_panel, dx, dy);
    Draw_OffsetCursor(4, 4);
    Draw_PrintTextById(0x6A);
    Draw_OffsetCursor(0x3C, 0);
    Draw_PrintTextById(0x6B);

    sum = g_StatBaseTable[0] + g_StatBaseTable[2];
    if (sum < 100 && !g_MenuEquipMode) {
        Draw_OffsetCursor(-0xF, 3);
        Draw_PrintNumberWidth2Unk(sum);
    } else {
        Draw_OffsetCursor(-0x14, 3);
        displayed = 999;
        if (sum < 1000) displayed = sum;
        Draw_PrintNumberWidth3Unk(displayed);
    }

    sum = g_StatBaseTable[1] + g_StatBaseTable[3];
    if (sum < 100 && !g_MenuEquipMode) {
        Draw_OffsetCursor(0x32, 0);
        Draw_PrintNumberWidth2Unk(sum);
    } else {
        Draw_OffsetCursor(0x2D, 0);
        displayed = 999;
        if (sum < 1000) displayed = sum;
        Draw_PrintNumberWidth3Unk(displayed);
    }
}

int Menu_InventoryPageInputHandler(MenuWidgetNode *root, u32 flags) {
    MenuWidgetNode *widget;
    MenuWidgetNode *node;
    int handled;

    handled = 0;
    widget = MenuWidget_GetChild(root, 0);
    node = MenuWidget_FindByModeAndSelectedBase(2, 0xB);

    if (node != 0) {
        handled = Menu_InventoryNavigate(widget, node, flags);
    }

    if (handled == 0 && (flags & 0x40)) {
        if (node != 0) {
            return 1;
        }

        MenuWidget_NavScrollTo(7);
        MenuWidget_DestroyNode(root);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(0x1C);
        Menu_CreateBonusPointAllocationView();
        Menu_PlayCancelSound();
        return 1;
    }

    if (flags & 0x1000) {
        widget->cursor_x = -1;
        if (node == 0) {
            widget = MenuWidget_FindByModeAndSelectedBase(2, 5);
        } else {
            widget = MenuWidget_FindByModeAndSelectedBase(2, 0x1B);
        }

        if (widget != 0) {
            widget->cursor_x = 0;
            widget->cursor_y = widget->y_limit - 1;
            MenuWidget_SetCurrentNode(widget);
        }
        Menu_PlayMoveSound();
    }

    return 1;
}
