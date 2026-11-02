/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_record.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_memcard_display.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_MemCardProgressPrimList;
void (*g_MenuDeferredCallback)(void);
ItemDataRecord *g_MenuSelectedItemData;
int g_MenuSelectionLocked;
MenuWidgetNode *g_MenuActiveWidget;
ItemDataRecord *g_MenuActionItemData;
int g_InvItemUsableFlag;
int g_MenuInputPausePending;
int g_MenuEquipMode;
int g_MenuItemRenameMode;
int g_SaveSelectedSlot;
int g_SaveSlotCellIndex;
int g_MenuItemContextFlag;
int g_MenuItemActionContext;
int g_MenuActiveItemSlot;
int g_MenuItemActionDisabled;
int g_MenuActionTextBase;
int g_MenuLayoutLocked;
unsigned char *g_MenuItemIconGlyphTable;

void Menu_DrawSaveSlotList(int arg0) {
    g_MenuActiveWidget = (MenuWidgetNode *)arg0;
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawSaveSlotEntry);
    Draw_BlendColor(g_SavedDrawBlendColor[0]);
    Menu_SetSaveSlotBlendBase(0);
}

/* Contiguous list-draw adapters: each binds a widget list to its row drawer,
 * plus the icon, save-slot, sound-effect and equip-detail lists and the
 * equip sort-toggle and info-option rows. */

void Menu_DrawItemIcon(int index);
void Menu_DrawSaveSlot1Item(int index);
void Menu_DrawSaveSlot2Item(int index);
void Menu_DrawSaveSlot3Item(int index);
void Menu_DrawSoundEffectItem(int index);
void Menu_DrawActionCodeItem(int index);

void Menu_DrawMainMenuItem(int arg0);

void Menu_DrawMainMenuList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawMainMenuItem);
}

void Menu_DrawActionSubmenuItem(int arg0);

void Menu_DrawActionSubmenuList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawActionSubmenuItem);
}

void Menu_DrawCursorModeItem(int arg0);

void Menu_DrawCursorModeList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawCursorModeItem);
}

void Menu_DrawNotificationItem(int index);

void Menu_DrawNotificationDialogContent(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawNotificationItem);
}

void Menu_DrawInvPanelItemStub(int index);

void Menu_DrawItemListInvPanel(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawInvPanelItemStub);
}

void Menu_DrawStatAllocationItem(int arg0);

void Menu_DrawStatAllocationList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawStatAllocationItem);
}

int Menu_GetStatBaseValue(int index) {
    return g_StatBaseTable[index];
}

void Menu_DrawScreenModeItem(int arg0);

void Menu_DrawScreenModeList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawScreenModeItem);
}

void Menu_DrawParasiteAbilityItem(int arg0);

void Menu_DrawParasiteAbilityList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawParasiteAbilityItem);
}

void Menu_DrawEmptyList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, 0);
}

void Menu_DrawItemIconList(MenuWidgetNode *node) {
    MenuWidgetNode *entry;
    int index;

    entry = MenuWidget_FindByModeAndSelectedBase(2, 0x17);
    if (entry != 0) {
        index = entry->cursor_y + 0x73;
    } else {
        index = 0x75;
    }

    g_MenuItemIconGlyphTable = Text_LookupTable4(index);
    MenuWidget_DrawList(node, Menu_DrawItemIcon);
}

void Menu_DrawMemCardSlot1List(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSaveSlot1Item);
    MenuWidget_GetCurrentNode();
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
}

void Menu_DrawMemCardSlot2List(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSaveSlot2Item);
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
}

void Menu_DrawMemCardSlot3List(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSaveSlot3Item);
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
}

void Menu_DrawSoundEffectList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawSoundEffectItem);
}

void Menu_DrawEquipItemDetailPanel(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    g_MenuActionItemData = g_MenuSelectedItemData;
    MenuWidget_DrawList(node, Menu_DrawActionCodeItem);
}

void Menu_RebuildSelectableMask(void) {
    Inv_RebuildSelectableMask();
}

void Menu_ActivateItemPickSlot(int unused, int doIt) {
    MenuWidgetNode *node;
    MenuWidgetNode *next;

    if (doIt != 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 7);
        if (g_InvItemUsableFlag != 0) {
            Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(node));
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x >= 0 ? next : MenuWidget_FindByModeAndSelectedBase(2, 6));
            MenuWidget_NavScrollTo(0x35);
        } else if (Inv_GetSlotItemData(MenuWidget_GridCellIndex(node)) == 0) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x >= 0 ? next : MenuWidget_FindByModeAndSelectedBase(2, 6));
            MenuWidget_NavScrollTo(0x35);
        }
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node == MenuWidget_GetCurrentNode()) {
            node->cursor_y = 0;
            node->cursor_x = 0;
        }
        if (g_InvItemUsableFlag != 0) {
            MenuInput_SetPollingPaused(1);
            g_MenuInputPausePending = 1;
        }
    }
}

void Menu_DrawEquipSortToggleRow(int arg0) {
    short *p;

    Draw_OffsetCursor(0, 2);
    Draw_AllocSprite(g_MenuSortToggleGlyphs[arg0]);
    Draw_OffsetCursor(0x12, 4);
    Draw_AllocSprite(0x22);
    Draw_OffsetCursor(0x0C, -4);

    p = g_MenuSortToggleGlyphs;
    if (arg0 == 0) {
        p++;
    }
    Draw_AllocSprite(*p);
}

void Menu_DrawEquipInfoOptionRow(int arg0) {
    if (g_InvItemUsableFlag != 0) {
        Draw_AllocSprite(arg0 + 0x7C);
    } else {
        Draw_AllocSprite(arg0 + 0x7F);
    }
}

int Inv_GetPackedListItem(int arg0);
void Menu_CloseContextHelpPanel(void);
int Inv_GetActiveListItem(int arg0);
void Draw_SetColor(int arg0);

void Menu_ItemUseAction(struct MenuWidgetNode *arg0, int arg1) {
    int saved;
    ItemDataRecord *ptr;

    if (arg1 != 0) {
        saved = Inv_GetPackedListItem(MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 0xD)));
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
        Menu_CloseContextHelpPanel();
        ptr = Inv_LookupActiveListData(saved);
        ptr->tailCount++;
        Menu_CreateItemUsePanel(Inv_GetActiveListItem(saved));
    }
}

void Menu_DrawBlendColorChannelRow(int arg0) {
    int shift;
    int value;

    Draw_StatePush();
    shift = arg0 << 3;
    Draw_SetColor(0x80 << shift);
    Draw_PrintTextById(arg0 + 0x35);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(0x0A, 3);
    value = (Draw_GetBlendColor() >> shift) & 0xFF;
    Draw_PrintNumberWidth3Unk((value - 0x20) >> 1);
    Draw_StatePop();
}

void MenuWidget_DrawCenteredTableText(int arg0);

void Menu_DrawBlendColorOptionRow(int arg0) {
    Draw_OffsetCursor(0, 1);
    MenuWidget_DrawCenteredTableText(arg0 + 0x33);
}

/* Save-slot write and read starts and the two confirmation handlers that
 * launch the card write behind the progress widget or report the error. */

void Save_StartWriteSlot(int arg0, int arg1);
int Save_StartReadSlot(int port, int slot);
void MemCard_SetDelayedCallback(void (*callback)(void));
void MemCard_DrawProgress(void);
int Menu_MemCardProgressInputHandler(void);
void MemCard_MarkActivePortState13(void);
void MemCard_ClearActivePrompt(void);
int MemCard_GetActivePort(void);

void Save_StartSelectedWriteSlot(void) {
    Save_StartWriteSlot(g_SaveSelectedSlot, g_SaveSlotCellIndex);
}

void Save_StartSelectedReadSlot(void) {
    Save_StartReadSlot(g_SaveSelectedSlot, g_SaveSlotCellIndex);
}

void Menu_TriggerSaveWrite(MenuWidgetNode *arg0, int arg1) {
    if (arg1 != 0) {
        MenuWidget_NavScrollTo(0x1F);
        Menu_CreateMemCardProgressWidget(0x45);
        MemCard_SetDelayedCallback(Save_StartSelectedWriteSlot);
    }
}

void Menu_HandleMemCardWriteOrError(MenuWidgetNode *arg0, int arg1) {
    MenuWidgetNode *entry;

    if (arg1 != 0) {
        entry = MenuWidget_CreateSimpleNode(0x27, MenuWidget_GetCurrentNode(), 0, 1);
        entry->draw = MemCard_DrawProgress;
        entry->update = Menu_MemCardProgressInputHandler;
        MenuWidget_SetCurrentNode(entry);
        g_MemCardProgressPrimList = 0x40;
        MemCard_SetDelayedCallback(MemCard_MarkActivePortState13);
    } else {
        Menu_CreateTwoLineDialog(MemCard_GetActivePort() + 0x47, 0x4B);
        g_MenuDeferredCallback = MemCard_ClearActivePrompt;
    }
}

/* Row drawers for the equip-slot, inventory, armor, battle-command and
 * skill lists. */

void Menu_DrawEquipSlotListItem(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }

    Draw_OffsetCursor(-2, -2);

    if (g_MenuEquipMode != 0) {
        Draw_AllocSprite(g_MenuEquipSlotGlyphs[g_MenuEquipMode][arg0]);
    }
}

void Menu_DrawInventoryItemRow(int index) {
    int slot = Inv_GetWayneListItemByIndex(index);

    Draw_SetTextDimmed(Inv_TestSelectionBit(slot) == 0);
    Menu_DrawActiveListSlot(slot);
    if (Inv_IsAyaInventorySlotReserved(slot) != 0) {
        Draw_DropShadow();
    }
}

void Menu_DrawArmorItemRow(int index) {
    Menu_DrawArmorItemDetail(index);
}

void Menu_DrawBattleCommandItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x1F);
}

void Menu_DrawSkillItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x5D);
}

void Draw_SetTextDimmed(int value);

void Menu_DrawItemSlot(int arg0) {
    int var_s0;
    int var_s1;
    int mask;
    int index;

    var_s1 = arg0;

    if (Menu_GetBattleEquipMode() != 0) {
        mask = g_MenuItemContextFlag & 0x1F;
    } else {
        mask = g_MenuItemContextFlag & 0x1EF;
    }

    index = -1;
    while (index < 9 && var_s1 >= 0) {
        var_s1 -= mask & 1;
        mask >>= 1;
        index++;
    }

    var_s0 = index;
    if (arg0 == MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        if (var_s0 == 4) {
            if (Battle_IsActiveWrapped() == 0) {
                Draw_SetTextDimmed(1);
            }
        }
    } else {
        Draw_SetTextDimmed(1);
    }

    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(var_s0 + 0x56);
}

void Menu_DrawSelectableEquipSlotRow(int slot) {
    int dimmed = 0;

    if (g_MenuSelectionLocked != 0 || Inv_TestSelectionBit(slot) == 0) {
        dimmed = 1;
    }

    Draw_SetTextDimmed(dimmed);
    Menu_DrawActiveListSlot(slot);
    if (Inv_IsAyaInventorySlotReserved(slot) != 0) {
        Draw_DropShadow();
    }
}

void Menu_ContextPick(int arg0)
{
    int state;
    int flag;
    int raw;
    ItemDataRecord *entry;

    state = g_MenuItemActionStateTable[g_MenuItemActionContext][arg0];

    switch (state) {
    case 0:
        flag = 0;
        raw = g_MenuActiveItemSlot;
        entry = Inv_LookupActiveListData(raw);
        if (Inv_TestSelectionBit(raw) != 0) {
            if (g_MenuEquipMode != 1 || entry->kind != 0xA || (u8)entry->bonusStats[0] < 4) {
                flag = 1;
            }
        }
        Draw_SetTextDimmed(flag ^ 1);
        break;
    case 1:
        break;
    case 2:
        Draw_SetTextDimmed(Inv_IsSlotSelectable(g_MenuActiveItemSlot) == 0);
        break;
    case 3:
        Draw_SetTextDimmed(g_MenuItemActionDisabled);
        break;
    }
    MenuWidget_DrawCenteredText(Text_LookupTable4(state));
}

/* Per-row item drawers the widget list renderer calls with the row index:
 * action options, equipped weapon/armor, action codes, modifier names, armor
 * cells, table text rows, stat and ability bars, item icons, save slots and
 * the selected item's sound-effect cursor. */

void Draw_PrintNumberWidth2(int arg0);
void Draw_PrintNumberWidth2Unk(int);
void Menu_UpdateStatBarAnimation(int);
void Draw_AllocTexturedQuad(int);
void Inv_ShowItemDescriptionPanel(void);

void Menu_DrawActionOptionRow(int arg0) {
    Draw_SetTextDimmed(0);
    MenuWidget_DrawCenteredTableText(g_MenuActionTextBase + arg0);
}

void Menu_SoundTestSelect(int index)
{
    int selected;
    int value;
    MenuWidgetNode *node;

    if (g_MenuLayoutLocked != 0) {
        selected = Inv_RestoreSelection(0);
    } else {
        Draw_SetTextDimmed(Battle_IsInputAllowedWrapped() == 0);
        if (g_InvItemUsableFlag != 0) {
            value = g_AyaEquippedWeaponSlot[0];
        } else {
            value = g_AyaEquippedArmorSlot[0];
        }
        selected = value;
    }

    if (selected >= 0) {
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
        Menu_DrawActiveListSlot(selected);
        return;
    }

    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
    node = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (node->cursor_x >= 0) {
        node->cursor_x = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 5);
        node->cursor_x = 0;
        MenuWidget_SetCurrentNode(node);
    }
    Draw_PrintTextById(0x39);
}

void Menu_DrawActionCodeItem(int arg0) {
    int code;

    if (arg0 < g_MenuActionItemData->tailCount) {
        code = g_MenuActionItemData->tailData[arg0] & 0x1F;
        if (code == 0) {
            return;
        }

        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(code + 0x22);
        } else {
            Draw_AllocSprite(code + 0x36);
        }
    } else {
        Draw_AllocSprite(0x46);
    }
}

void Menu_DrawModNameItem(int arg0) {
    if (arg0 < Inv_GetPackedListCount()) {
        Draw_PrintRawText(Text_LookupTable8(Inv_GetPackedListItem(arg0) + 0xEB));
    }
}

void Menu_DrawArmorListItem(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(arg0 + 0x60);
}

void Menu_DrawArmorEquipItem(int index) {
    Inv_ShowItemDescriptionPanel();
}

void Menu_DrawMemCardPortItem(int arg0) {
    Draw_OffsetCursor(-2, -2);
    if (arg0 < 2) {
        Draw_AllocSprite(arg0 + 0x84);
    } else {
        Draw_AllocSprite(0x62);
    }
}

void Menu_DrawMainMenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x28);
}

void Menu_DrawActionSubmenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x2E);

    if (Menu_GetActionSubmenuSelection() == arg0) {
        Draw_DropShadow();
    }
}

void Menu_DrawCursorModeItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x31);

    if (MenuWidget_GetColumnLayoutMode() == arg0) {
        Draw_DropShadow();
    }
}

void Menu_DrawNotificationItem(int index) {
    MenuWidget_DrawCenteredTableText(8);
}

void Menu_DrawInvPanelItemStub(int index) {
}

void Menu_DrawStatAllocationItem(int arg0) {
    int value;
    int local;

    arg0 += 1;
    value = g_BonusPointStatDeltas[arg0] + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[arg0]) >> 7);

    Draw_OffsetCursor(6, 0);
    Draw_AllocSprite(arg0 + 0x8C);
    Draw_OffsetCursor(0x4A, 0);
    Aya_StatQueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
}

void Menu_DrawScreenModeItem(int arg0) {
    Draw_StatePush();
    Draw_PrintTextById((arg0 & 1) | 0x6A);

    if ((arg0 >= 2) || (g_StatBaseTable[arg0] >= 100)) {
        Draw_OffsetCursor(0x28, 3);
        Draw_PrintNumberWidth3Unk(g_StatBaseTable[arg0]);
    } else {
        Draw_OffsetCursor(0x2D, 3);
        Draw_PrintNumberWidth2Unk(g_StatBaseTable[arg0]);
    }

    Draw_StatePop();
}

void Menu_DrawParasiteAbilityItem(int arg0) {
    int value;
    int local;

    value = g_BonusPointStatDeltas[arg0 + 5];

    Draw_OffsetCursor(2, 1);
    Draw_AllocSprite(arg0 + 0x91);
    Draw_OffsetCursor(0x42, -1);

    arg0 += 5;
    Aya_StatQueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
    Draw_OffsetCursor(2, 0);
    Menu_UpdateStatBarAnimation(arg0);
    Draw_OffsetCursor(-0x44, 0xE);
}

void Menu_DrawItemIcon(int arg0) {
    Draw_StatePush();
    Draw_AllocTexturedQuad(g_MenuItemIconGlyphTable[arg0]);
    Draw_StatePop();
}

void Menu_DrawSaveSlot1Item(int index) {
    if (index != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x63);
}

void Menu_DrawSaveSlot2Item(int index) {
    if (index != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x66);
}

void Menu_DrawSaveSlot3Item(int index) {
    if (index != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x62);
}

void Menu_DrawSoundEffectItem(int index) {
    Menu_CursorRenderData(g_MenuSelectedItemData);
}
