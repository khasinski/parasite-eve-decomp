/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_inventory_root.h"
#include "common.h"
#include "pe1/menu_inventory.h"
#include "include_asm.h"

/* Defined here so the assembler sees a small-data symbol: retail waits a
 * load-delay slot before the gp-relative store of the reserve ammo. */
int D_8009CFCC;

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
    if (D_8009CF0C) {
        panel = MenuWidget_CreateSimpleNode(0x36, MenuWidget_GetCurrentNode(), 0, 0);
        node = MenuWidget_CreateNode(0x36, panel, panel);
        panel->update = Menu_InventoryPageNavHandler;
        if (D_8009CF30)
            MenuWidget_OffsetPosition(panel, 0, 0x14);
        node->draw = Menu_DrawBattleCommandList;
        MenuWidget_SetCurrentNode(node);
    }
    panel = MenuWidget_CreateSimpleNode(0xD, MenuWidget_GetCurrentNode(), 0, 0);
    node = MenuWidget_CreateNode(D_8009CF30 ? 0x10 : 0xD, panel, panel);
    panel->update = Menu_ItemListInputHandler;
    panel->disabled = 1;
    node->draw = Menu_DrawEquipSelectionList;
    if (D_8009CF1C) {
        MenuWidget_SetColumnLayout(node, 0x15);
        if (node->cursor_x < 0)
            node->cursor_x = 0;
        MenuWidget_ClampScroll(node);
    }
    if (D_8009CF0C) {
        MenuWidget_OffsetPosition(panel, 0, D_8009CF30 ? -0x10 : 0x14);
        node->cursor_x = -1;
    }
    D_8009CFD4 = Inv_IsActiveListOverrideSelected();
    Inv_SelectActiveList(0);
    D_8009CFC4 = list;
    D_8009CF34 = list == -2;
    D_8009CF38 = list == -3;
    if (D_8009CF34) {
        MenuWidget_ClearColumnLayout(node);
        node->cursor_x = 0;
    }
    if (D_8009CF1C)
        Inv_BuildFilteredPackedListExcluding(mode, -1);
    else if (!D_8009CF34 && !D_8009CF38)
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
    if (node->cursor_x >= 0 && !D_8009CF0C)
        MenuWidget_SetCurrentNode(node);
    if (D_8009CF30) {
        MenuWidget_OffsetPosition(panel, 0, 0x38);
        panel->visible_rows -= 0x30;
        panel = MenuWidget_CreateSimpleNode(0x18, 0, 0, 0);
        panel->draw = Menu_DrawBonusPointSlotValue;
        panel->grid_width += 0x14;
        MenuWidget_OffsetPosition(panel, -0x88, 0);
        if (!D_8009CF0C) {
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
            u16 *source = D_800C0E28;
            int stat;

            for (stat = 0; stat < 7; stat++) {
                D_800A18D8[stat] = *source++;
                D_800A18FC[stat] = 0;
                Stat_QueryLevelAndSubLevel(stat, D_800A18D8[stat], &D_800A18B4[stat], 0);
            }
        }
        D_8009CF80 = 0;
        D_8009CF40 = 0;
        D_8009CF68 = D_800C0E10[0];
        for (slot = 6; slot >= 0; slot--)
            D_800A1898[slot] = 0;
        Menu_InitStateTables();
    }
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    if (D_8009CF1C && !MenuWidget_FindByModeAndSelectedBase(1, 0x2F))
        MenuWidget_CreateSimpleNode(0x2F, 0, 0, 0)->draw = Menu_DrawItemDetailPanel;
    panel = MenuWidget_CreateSimpleNode(0xB, node, 0, 0);
    node = MenuWidget_CreateNode(0xB, panel, panel);
    panel->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    MenuWidget_ClearColumnLayout(node);
    MenuWidget_OffsetPosition(panel, 6, 0);
    panel = MenuWidget_CreateSimpleNode(0xF, node, 0, 0);
    if (D_8009CF30) {
        node = MenuWidget_CreateNode(0x1D, panel, panel);
        panel->update = Menu_BonusPointCancelHandler;
        node->draw = Menu_DrawEmptyList;
        node->cursor_x = -1;
        D_800A1960[0] = 0;
    }
    panel->draw = Menu_StepEquipSlotSelect2;
    if (data)
        D_8009CFC8 = (u8)data->bonusStats[0] & 3;
    else
        D_8009CFC8 = 0;
    if (data)
        D_8009CFCC = data->reserveAmmo;
    else
        D_8009CFCC = 0;
    D_8009CF18 = mode != 0x200;
    node = MenuWidget_GetCurrentNode();
    if (node->selected_base == 0x30 || node->selected_base == 0x36)
        Menu_SetEquipPanelsCursorY();
    else
        Menu_AlignEquipPanels();
}

void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);

void Menu_AlignEquipPanels(void) {
    register s32 temp_s1 asm("$17");
    MenuWidgetNode *temp_s0;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
    MenuWidgetNode *child;
    register int x asm("$5");
    register int target asm("$2");

    temp_s0 = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
    __asm__ volatile("" : "=r"(temp_s0) : "0"(temp_s0));
    child = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
    target = g_MenuItemUseMode;
    x = temp_s0->x;
    if (target != 0) {
        target = 0xB0;
        goto aligned;
    }
    target = (s32)child->popup_node;
    if (target != 0) {
        target = 0xA2;
        goto aligned;
    }
    target = 0x9C;
aligned:
    temp_s1 = target - x;
    asm("" : : "r"(temp_s1));
    MenuWidget_OffsetPosition(temp_s0, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_s0);
    temp_v0 = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    MenuWidget_OffsetPosition(temp_v0, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_v0);
    temp_v0_2 = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
    MenuWidget_OffsetPosition(temp_v0_2, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_v0_2);
}

void MenuWidget_SetCursorY(MenuWidgetNode *node);

void Menu_SetEquipPanelsCursorY(void) {
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xF));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xB));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x2F));
}

int g_MenuItemUseMode;
int g_BonusPointDisplayValue;

void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int arg0);
void Draw_PrintNumberWidth5(int arg0);

void Menu_DrawBonusPointSlotValue(void) {
    int value;

    Draw_OffsetCursor(6, 4);
    Draw_AllocSprite(0x93);
    value = g_MenuItemUseMode;
    Draw_OffsetCursor((value * 5 * 4) + 0x48, 0);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
}

void Menu_CreateBonusPointAllocationView(void);
void Inv_RecalcSlotStats(s32 arg0, void *arg1);
void MenuWidget_NavScrollTo(s32 selected_base);

extern s32 g_MenuBattleStatusOverlayActive;
extern s32 g_MenuEquipMode;
extern s32 g_MenuItemRenameMode;
extern u16 g_BonusPointStatDeltas[];
#define g_BonusPointStatDeltas (g_BonusPointStatDeltas[0])
extern u16 g_AyaStatAgility[];
#define g_AyaStatAgility (g_AyaStatAgility[0])
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])

void Menu_ExitBonusPointAllocation(void) {
    s32 index;
    u8 *dst;
    u8 *src;
    u16 value;

    dst = (u8 *)&g_AyaStatAgility;
    index = 0;
    src = (u8 *)&g_BonusPointStatDeltas;
    do {
        value = *(u16 *)src;
        src += 4;
        index += 1;
        *(u16 *)dst = value;
        dst += 2;
    } while (index < 7);
    g_AyaBonusPoints = g_BonusPointDisplayValue;
    Inv_RecalcSlotStats(index, dst);
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
        MenuWidgetNode *other;
        int x, offset;

        current->cursor_x = -1;
        current = MenuWidget_FindByModeAndSelectedBase(2, 16);
        current->cursor_x = 0;
        current->cursor_y = 0;
        MenuWidget_ClampScroll(current);
        MenuWidget_SetCurrentNode(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 15);
        other = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 13), 0);
        x = current->x;
        offset = (g_MenuItemUseMode ? 176 : other->popup_node ? 162 : 156) - x;
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 11);
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 47);
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        Menu_OpenBonusPointSpendDialog(current, MenuWidget_GridCellIndex(current) + 5);
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        func_800490B0();
        Menu_PlayCancelSound();
    }
    return 1;
}
