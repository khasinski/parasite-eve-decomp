/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/game_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/memcard_save_state.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_flags.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/Menu_MemCardProgress_externs.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/save.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_SaveSelectedSlotState;
int g_InvItemUsableFlag;
int g_InvSelectedItemIndex;
ItemDataRecord *g_MenuActionItemData;
int g_MenuActionTextBase;
int g_MenuActiveItemList;
MenuConfirmCallback g_MenuConfirmCallback;
void (*g_MenuDeferredCallback)(void);
int g_MenuEquipMode;
int g_MenuItemContextFlag;
unsigned char *g_MenuItemIconGlyphTable;
int g_MenuItemPrimListHandles[2];
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
ItemDataRecord *g_MenuRenameTargetRecord;
int g_SaveSelectedSlot;
int g_SaveSlotCellIndex;
int g_McDialogMode;
int g_MenuMemCardConfirmPending;
int g_MenuSpellInfoId;
int g_MemCardProgressPrimList;

/* Parasite-spell info and context help panels, notification and two-line
 * dialogs, the deferred confirm callback they run, and the memory-card port
 * dialog with its error prompt. */

#define NULL ((void *)0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

s32 Menu_MemCardPortSelectHandler(void *node, s32 flags);
void Menu_DrawParasiteSpellInfoPanel(void);

/* JP-only panel polygon for the spell info window (D_80091E14). */

void Menu_CreateParasiteSpellInfoPanel(int arg0) {
    MenuWidgetNode *widget;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0x1E) == 0) {
        widget = MenuWidget_CreateSimpleNode(0x1E, 0, 0, 0);
        widget->draw = Menu_DrawParasiteSpellInfoPanel;
        widget->appearance.gradientPoints = g_MenuSpellInfoPanelPoints;
    }

    g_MenuSpellInfoId = arg0;
}

void Menu_DrawParasiteSpellInfoPanel(void) {
    Draw_OffsetCursor(2, 4);
    Draw_PrintCenteredText(Text_LookupTable4(0x26));
    Draw_OffsetCursor(0, 0x10);
    Draw_PrintCenteredText(Text_LookupTable8(g_MenuSpellInfoId + 0xEB));
    Draw_OffsetCursor(2, 0x14);
    Draw_PrintTextWrapped(Text_LookupTableC(g_MenuSpellInfoId + 0xEB), 0xAA);
}

void Menu_CreateContextHelpPanel(void) {
    void *node = MenuWidget_FindByModeAndSelectedBase(1, 0x13);

    if (node == 0) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = Menu_DrawContextHelpText;
    }
}

void Menu_CloseContextHelpPanel(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
}

/* Draws the play-time clock and the help line for the focused menu widget:
 * the item description, ability or option hint selected by its grid cell. */
void Menu_DrawContextHelpText(MenuWidgetNode *panel)
{
    MenuWidgetNode *current;
    ItemDataRecord *data;
    u8 *text;
    int index;
    int mask;
    int value;
    int hint;
    int i;

    text = 0;
    current = MenuWidget_GetCurrentNode();
    Draw_StatePush();
    Draw_OffsetCursor(panel->grid_width - 0x27, 1);
    Draw_PrintTimeValue(GameTime_GetCounterSeconds(2), 1);
    Draw_OffsetCursor(-0x21, 0);
    Draw_AllocSprite(0x8C);
    Draw_StatePop();
    if (current != 0) {
        index = MenuWidget_GridCellIndex(
            MenuWidget_FindByModeAndSelectedBase(2, current->selected_base));
        switch (current->selected_base) {
        case 0:
            if (Menu_GetBattleEquipMode())
                mask = g_MenuItemContextFlag & 0x1F;
            else
                mask = g_MenuItemContextFlag & 0x1EF;
            i = -1;
            do {
                int bit;

                if (index < 0)
                    break;
                bit = mask & 1;
                index -= bit;
                i++;
                mask >>= 1;
            } while (i < 9);
            text = Text_LookupTable10(i + 0x28);
            break;
        case 5:
            if (index < 0)
                break;
            data = Inv_LookupActiveListData(g_InvItemUsableFlag ? g_InventoryRuntime.tracked[0]
                                                       : g_InventoryRuntime.tracked[2]);
            if (data != 0)
                text = Text_LookupTableC(data->itemId - 1);
            break;
        case 1:
            if (g_InvSelectedItemIndex >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(g_InvSelectedItemIndex) < 0x13
                    || Inv_GetActiveListItemType(g_InvSelectedItemIndex) >= 0x16)
                    hint = 1;
                text = Text_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Text_LookupTableC(data->itemId - 1);
            break;
        case 51:
            if (g_InvSelectedItemIndex >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(g_InvSelectedItemIndex) < 0x13
                    || Inv_GetActiveListItemType(g_InvSelectedItemIndex) >= 0x16)
                    hint = 1;
                text = Text_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupActiveListData(Inv_GetWayneListItemByIndex(index));
            if (data != 0)
                text = Text_LookupTableC(data->itemId - 1);
            break;
        case 52:
            if (g_InvSelectedItemIndex >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(g_InvSelectedItemIndex) < 0x13
                    || Inv_GetActiveListItemType(g_InvSelectedItemIndex) >= 0x16)
                    hint = 1;
                text = Text_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupItemData(index);
            if (data != 0)
                text = Text_LookupTableC(data->itemId - 1);
            break;
        case 7:
            if (g_MenuLayoutLocked)
                index = Inv_RestoreSelection(1);
            else
                index = Inv_GetPackedListItem(index);
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Text_LookupTableC(data->itemId - 1);
            break;
        case 8:
            if (index < Inv_GetPackedListCount())
                text = Text_LookupTableC(Inv_GetPackedListItem(index) + 0xEB);
            break;
        case 12:
            text = Text_LookupTable10(index + 0x4F);
            break;
        case 13:
        case 16:
            if (g_MenuSelectionLocked) {
                text = Text_LookupTableC(Menu_GetBattleCountEntry(index) - 1);
                break;
            }
            data = Inv_LookupActiveListData(Inv_GetPackedListItem(index));
            if (data != 0)
                text = Text_LookupTableC(data->itemId - 1);
            break;
        case 14:
            Inv_SelectActiveList(0);
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Text_LookupTableC(data->itemId - 1);
            break;
        case 6:
            value = g_MenuActionItemData->tailData[index] & 0x1F;
            if (value != 0)
                text = Text_LookupTable10(value + (1 - g_InvItemUsableFlag) * 20 - 1);
            break;
        case 11:
            data = Inv_LookupActiveListData(Inv_RestoreSelection(1));
            value = data->tailData[index] & 0x1F;
            if (value != 0)
                text = Text_LookupTable10(value + (1 - g_InvItemUsableFlag) * 20 - 1);
            break;
        case 17:
            text = Text_LookupTable4(0x27);
            break;
        case 27:
        case 28:
            text = Text_LookupTable10(0x52);
            break;
        case 29:
            {
                int base = g_InvItemUsableFlag * 3 - 0x56;

                text = Text_LookupTable10(index - base);
            }
            break;
        case 32:
            if (index < 4)
                text = Text_LookupTable10(index + 0x31);
            else
                text = 0;
            break;
        case 33:
            text = Text_LookupTable10(index + 0x3A);
            break;
        case 35:
            text = Text_LookupTable10(index + 0x3D);
            break;
        case 36:
            if (MemCard_PollTransferDelay()) {
                Draw_OffsetCursor(4, 5);
                Draw_PrintTextById(0x4C);
                Draw_OffsetCursor(0, 0x10);
                Draw_PrintTextById(0x4D);
                break;
            }
            hint = 0;
            if (MemCard_IsPortPresent(0) || MemCard_IsPortPresent(1))
                hint = 1;
            text = Text_LookupTable4(hint | 0x4E);
            break;
        case 37:
        case 38:
            if (g_McDialogMode) {
                if (g_MenuMemCardConfirmPending) {
                    Draw_SetTextDimmed(0);
                    Draw_OffsetCursor(4, 5);
                    Draw_PrintTextById(0x58);
                    Draw_OffsetCursor(0, 0x10);
                    Draw_PrintTextById(0x59);
                    break;
                }
                text = Text_LookupTable4(0x5C);
                break;
            }
            text = Text_LookupTable4(Save_GetTitleStyleFlag() ? 0x57 : 0x5B);
            break;
        case 39:
            Draw_SetTextDimmed(0);
            Draw_OffsetCursor(4, 5);
            Draw_PrintTextById(0x50);
            Draw_OffsetCursor(0, 0x10);
            Draw_PrintTextById(0x51);
            break;
        case 46:
            text = Text_LookupTable10(index + 0x3F);
            break;
        case 48:
            text = Text_LookupTable10(index + 0x38);
            break;
        case 49:
            text = Text_LookupTable10(index + 0x42);
            break;
        case 50:
            /* Retail keeps the table base in a register for the row-4
             * override and recomputes the row address on both paths. */
            {
                u8 *hints = g_MenuContextHelpTextIds;
                u8 *hint;

                if (g_MenuEquipMode == 1) {
                    if (index == 2)
                        hint = &hints[4];
                    else
                        hint = &g_MenuContextHelpTextIds[index];
                } else {
                    hint = &g_MenuContextHelpTextIds[index];
                }
                text = Text_LookupTable10(*hint);
            }
            break;
        case 56:
            Draw_SetTextDimmed(0);
            text = Text_LookupTable10(0x44);
            break;
        case 58:
            text = Text_LookupTable10(index + 0x35);
            break;
        case 59:
            if (g_MenuEquipMode == 2)
                text = Text_LookupTable10(index + 0x4D);
            else
                text = Text_LookupTable10(index + 0x4B);
            break;
        case 60:
            {
                int base = g_InvItemUsableFlag * 3 - 0x48;

                text = Text_LookupTable10(index - base);
            }
            break;
        }
    }
    if (text != 0) {
        Draw_OffsetCursor(4, 5);
        Draw_PrintRawText(text);
        Draw_SetColor(0x808080);
    }
}

void Menu_CreateNotificationDialog(int message, int suffix) {
    MenuWidgetNode *parent, *child;
    short alternate;
    int mode, width;
    u8 *text, *messageText;
    alternate = MenuWidget_FindByModeAndSelectedBase(1, 0x28) != 0;
    mode = 0x28;
    if (alternate) {
        mode = 0x3D;
    }
    parent = MenuWidget_CreateSimpleNode(mode, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(mode, parent, parent);
    parent->draw = Menu_DrawNotificationText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->draw = Menu_DrawNotificationDialogContent;
    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);
    messageText = Text_LookupTable4(message);
    text = g_MenuTwoLineDialogText[alternate];
    Text_CopyFFTerminatedBytes(text, messageText);
    if (suffix) {
        Text_AppendFFTerminatedBytes(text, Text_LookupTable4(suffix));
    }
    width = Draw_MeasureTextWidth(text) < 100 ? 100 : Draw_MeasureTextWidth(text);
    parent->grid_width = width + 20;
    parent->x = (300 - width) >> 1;
    child->x = parent->grid_width - 68;
    MenuInput_InitQueue();
}

void Menu_CloseNotificationDialogs(void) {
    MenuWidget_NavScrollTo(0x28);
    MenuWidget_NavScrollTo(0x3D);
}

void Menu_DrawNotificationText(MenuWidgetNode *node) {
    u8 *text;

    Draw_OffsetCursor(0, 0xA);
    text = g_MenuTwoLineDialogText[0];
    if (node->selected_base == 0x3D) {
        text += 0x40;
    }
    Draw_PrintCenteredText(text);
}

void Menu_CreateTwoLineDialog(int line0_id, int line1_id) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
    u8 *line0;
    u8 *line1;
    u8 *text;
    int width;
    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->draw = Menu_DrawTwoLineDialogText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->draw = Menu_DrawNotificationDialogContent;

    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);

    text = Text_LookupTable4(line0_id);
    line0 = g_MenuTwoLineDialogText[0];
    Text_CopyFFTerminatedBytes(line0, text);

    text = Text_LookupTable4(line1_id);
    line1 = line0 + 0x40;
    Text_CopyFFTerminatedBytes(line1, text);

    width = MAX(100, MAX(Draw_MeasureTextWidth(g_MenuTwoLineDialogText[0]),
                         Draw_MeasureTextWidth(g_MenuTwoLineDialogText[1])));
    parent->grid_width = width + 0x14;
    parent->x = (0x12C - width) >> 1;
    parent->visible_rows += 0xE;
    child->x = parent->grid_width - 0x44;
    child->y += 0xE;

    MenuInput_InitQueue();
}

void Menu_DrawTwoLineDialogText(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText[0]);
    Draw_OffsetCursor(0, 0xE);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText[1]);
}

void Menu_SetDeferredCallback(void (*callback)(void)) {
    g_MenuDeferredCallback = callback;
}

int Menu_HandleDeferredCallbackInput(MenuWidgetNode *node, int arg1) {
    if ((arg1 & 0x10040) != 0) {
        MenuWidget_DestroyNode(node);
        if (g_MenuDeferredCallback != 0) {
            g_MenuDeferredCallback();
            g_MenuDeferredCallback = 0;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_OpenMemCardSelectDialog(s32 arg0) {
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_v0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x24, MenuWidget_GetCurrentNode(), 0, 0);
    temp_a0 = MenuWidget_CreateNode(0x24, temp_v0, temp_v0);
    temp_v0->update = Menu_MemCardPortSelectHandler;
    temp_a0->draw = Menu_DrawMemCardPortList;
    temp_a0->selectionAvailable = Menu_IsMemCardSlotSelectable;
    if (temp_a0->cursor_y == 2) {
        temp_a0->cursor_y = 0;
    }
    MenuWidget_SetCurrentNode(temp_a0);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = Menu_DrawContextHelpText;
    }
    MenuWidget_FindByModeAndSelectedBase(1, 0x13)->visible_rows = 0x24;
    g_McDialogMode = arg0;
    if (arg0 != 0) {
        MemCard_SetDialogActive(1);
        MemCard_InitState();
    }
    MenuInput_InitQueue();
}

void Menu_OpenStartupMemCardDialog(void) {
    s32 one;
    void *callback_2c;
    void *callback_30;
    void *callback_8c;
    MenuWidgetNode *child;
    MenuWidgetNode *parent;

    parent = MenuWidget_CreateSimpleNode(0x24, MenuWidget_GetCurrentNode(), 0, 0);
    child = MenuWidget_CreateNode(0x24, parent, parent);
    callback_2c = Menu_MemCardPortSelectHandler;
    callback_30 = Menu_DrawMemCardPortList;
    parent->update = callback_2c;
    child->draw = callback_30;
    callback_8c = Menu_IsMemCardSlotSelectable;
    child->selectionAvailable = callback_8c;
    one = 1;
    if (child->cursor_y == 2) {
        child->cursor_y = 0;
    }
    MenuWidget_SetCurrentNode(child);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = Menu_DrawContextHelpText;
    }
    MenuWidget_FindByModeAndSelectedBase(1, 0x13)->visible_rows = 0x24;
    g_McDialogMode = one;
    MemCard_SetDialogActive(1);
    MemCard_InitState();
    MenuInput_InitQueue();
}

unsigned int MemCard_GetDialogMode(void) {
    return g_McDialogMode;
}

void Menu_SetMemCardConfirmPending(void) {
    g_MenuMemCardConfirmPending = 1;
}

void Menu_ShowMemCardErrorDialog(int arg0) {
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
        Menu_CreateTwoLineDialog(arg0 + 0x47, g_McDialogMode + 0x42);
    }
}

s32 Menu_MemCardPortSelectHandler(void *node, s32 flags) {
    s32 cellIndex;

    cellIndex = MenuWidget_GetCellIndex(MenuWidget_GetChild(node, 0));
    if (flags & 0x10000) {
        switch (cellIndex) {
        case 0:
        case 1:
            if ((MemCard_CheckPresent(cellIndex) != 0) && (MemCard_IsPortPresent(cellIndex) != 0) && (MemCard_IsOperationPending() == 0)) {
                MemCard_CloseAllAndResetState();
                MemCard_StartRead(cellIndex, 1);
                Menu_PlayConfirmSound();
                return 1;
            }
            return 1;
        case 2:
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x25));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x26));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x24));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
            MemCard_SetDialogActive(0);
            Inv_SetActiveList(9, 0);
            g_MenuMemCardConfirmPending = 0;
            Menu_PlayConfirmSound();
            return 1;
        }
        Menu_PlayErrorSound();
        return 1;
    }
    if (flags & 0x40) {
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x25));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x26));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x24));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
        MemCard_SetDialogActive(0);
        Inv_SetActiveList(9, 0);
        g_MenuMemCardConfirmPending = 0;
        Menu_PlayCancelSound();
    }
    return 1;
}

int Menu_IsMemCardDialogOpen(void) {
    return MenuWidget_FindByModeAndSelectedBase(1, 0x24) != 0;
}

s32 Menu_CreateSaveSlotListView(s32 arg0, s32 arg1) {
    s32 temp_s0;
    s32 temp_s0_2;
    void *temp_a0;
    MenuWidgetNode *temp_s2;
    MenuWidgetNode *temp_v0;

    temp_s0_2 = arg0 + 0x25;
    temp_v0 = MenuWidget_CreateSimpleNode(temp_s0_2, MenuWidget_FindByModeAndSelectedBase(2, 0x24), 0, 0);
    temp_s2 = MenuWidget_CreateNode(temp_s0_2, temp_v0, temp_v0);
    temp_a0 = temp_s2;
    temp_v0->update = Menu_StepSaveSelect;
    temp_s2->draw = Menu_DrawSaveSlotList;
    temp_s2->selectionAvailable = Menu_IsMemCardSlotCompatible;
    Draw_SetPrimCallback(temp_a0, arg1);
    temp_s0 = MenuWidget_GridCellIndex(temp_s2);
    if (MenuWidget_FindLastMode1WithCursorX() == 0) {
        temp_s2->cursor_x = 0;
        MenuWidget_SetCurrentNode(temp_s2);
    } else {
        temp_s2->cursor_x = -1;
    }
    MenuWidget_CreateSimpleNode(0x3F, temp_s2, 0, 0)->draw = Menu_DrawSaveSlotPortLabel;
    MenuInput_InitQueue();
    g_SaveSelectedSlot = arg0;
    return temp_s0;
}

void Menu_CloseSaveSlotListView(s32 arg0) {
    MenuWidgetNode *temp_s0;

    temp_s0 = MenuWidget_FindByModeAndSelectedBase(2, 0x24);
    MenuWidget_NavScrollTo(0x28);
    if (g_MenuDeferredCallback != NULL) {
        g_MenuDeferredCallback();
        g_MenuDeferredCallback = NULL;
    }
    if (arg0 == g_SaveSelectedSlot) {
        MenuWidget_NavScrollTo(0x3F);
        MenuWidget_NavScrollTo(0x27);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x29));
        MenuWidget_NavScrollTo(0x1F);
        MenuWidget_NavScrollTo(arg0 + 0x25);
        if (temp_s0 != NULL) {
            temp_s0->cursor_x = 0;
            if (MenuWidget_FindLastMode1WithCursorX() == 0) {
                MenuWidget_SetCurrentNode(temp_s0);
            }
        }
    }
}

/* The port label under a save slot list; the list's parent selects the
 * port (selected_base 0x25 or 0x26). */
void Menu_DrawSaveSlotPortLabel(MenuWidgetNode *node) {
    Draw_OffsetCursor(0, 2);
    Draw_FlushPrimList(node->parent->selected_base - 0x11);
}

s32 Menu_StepSaveSelect(MenuWidgetNode *arg0, s32 arg1) {
    void (*var_a0)(void);
    s32 temp_s0;
    MenuWidgetNode *temp_s2;
    s32 temp_v0;
    s32 temp_cf50;
    s32 temp_height;
    s32 var_v1;
    s32 temp_a0;
    MemCardSaveSlot *temp_v0_2;
    MenuWidgetNode *temp_s2_2;
    MenuWidgetNode *temp_v0_3;

    temp_s2 = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x40) {
        MenuWidget_NavScrollTo(0x3F);
        MenuWidget_DestroyNode(arg0);
        MemCard_CloseAllAndResetState();
        Menu_PlayCancelSound();
        return 1;
    }
    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetCellIndex(temp_s2);
        g_SaveSlotCellIndex = temp_v0;
        if (temp_v0 >= 0) {
            temp_v0_2 = MemCard_GetSlot(g_SaveSelectedSlot, temp_v0);
            if (temp_v0_2 != NULL) {
                temp_cf50 = g_McDialogMode;
                if ((temp_cf50 != 0) || (temp_v0_2->state != MEMCARD_SLOT_EMPTY)) {
                    g_SaveSelectedSlotState = temp_v0_2->state;
                    if (temp_cf50 != 0) {
                        Save_SetTitleStyleFlag(g_MenuMemCardConfirmPending);
                        if (g_SaveSelectedSlotState != MEMCARD_SLOT_EMPTY) {
                            temp_v0_3 = MenuWidget_CreateSimpleNode(0x29, temp_s2, 0, 1);
                            temp_s2_2 = MenuWidget_CreateNode(0x29, temp_v0_3, temp_v0_3);
                            temp_v0_3->draw = Menu_DrawItemLabel;
                            temp_v0_3->update = Menu_ConfirmDialogHandler;
                            temp_s2_2->draw = Menu_DrawActionOptionList;
                            g_MenuActionTextBase = 0x6C;
                            MenuWidget_SetCurrentNode(temp_s2_2);
                            temp_a0 = g_MenuActiveItemList;
                            temp_s2_2->cursor_x = 1;
                            Inv_SelectActiveList(temp_a0);
                            g_MenuItemLabelBuffer[0] = 0xFF;
                            Text_AppendFFTerminatedBytes(g_MenuItemLabelBuffer, Text_LookupTable4(0x44));
                            g_MenuItemPrimListHandles[0] = 0;
                            {
                                MenuConfirmCallback callback = Menu_TriggerSaveWrite;
                                if (Draw_MeasureTextWidth(g_MenuItemLabelBuffer) < 0x78) {
                                    var_v1 = 0x78;
                                } else {
                                    var_v1 = Draw_MeasureTextWidth(g_MenuItemLabelBuffer);
                                }
                                temp_v0_3->grid_width = (var_v1 + 0x14);
                                temp_v0_3->visible_rows = 0x32;
                                temp_v0_3->x = ((0x12C - var_v1) >> 1);
                                temp_s2_2->x = ((temp_v0_3->grid_width - 0x80) >> 1);
                                temp_height = temp_v0_3->visible_rows;
                                g_MenuConfirmCallback = callback;
                                temp_s2_2->y = (temp_height - 0x14);
                            }
                        } else {
                            if (MemCard_GetPortSequence(g_SaveSelectedSlot) < 0xF) {
                                MenuWidget_NavScrollTo(0x1F);
                                Menu_CreateMemCardProgressWidget(0x45);
                                var_a0 = Save_StartSelectedWriteSlot;
                                MemCard_SetDelayedCallback(var_a0);
                            } else {
                                temp_s0 = g_SaveSelectedSlot;
                                if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
                                    Menu_CreateTwoLineDialog(temp_s0 + 0x47, g_McDialogMode + 0x42);
                                }
                            }
                        }
                    } else {
                        MenuWidget_NavScrollTo(0x1F);
                        Menu_CreateMemCardProgressWidget(0x46);
                        var_a0 = Save_StartSelectedReadSlot;
                        MemCard_SetDelayedCallback(var_a0);
                    }
                    Menu_PlayConfirmSound();
                    return 1;
                }
            }
        }
        Menu_PlayErrorSound();
        /* Duplicate return node #20. Try simplifying control flow for better match */
        return 1;
    }
    return 1;
}

/* Memory-card progress widget: creation, teardown, block-meter draw and the
 * write-confirmation prompt it raises when the card operation finishes. */

void MemCard_DrawProgress(void);
int Menu_MemCardProgressInputHandler(void);
void Draw_FlushPrimList(int arg0);
int MemCard_GetActiveProgressBlocks(void);
void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0);
int MemCard_GetActivePort(void);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void MemCard_ClearActivePrompt(void);

/* Separate compiler identities preserve address reloads across text calls. */

void Menu_CreateMemCardProgressWidget(int arg0) {
    void *node;
    MenuWidgetNode *widget;

    node = MenuWidget_GetCurrentNode();
    widget = MenuWidget_CreateSimpleNode(0x27, node, 0, 1);
    widget->draw = MemCard_DrawProgress;
    widget->update = Menu_MemCardProgressInputHandler;
    MenuWidget_SetCurrentNode(widget);
    g_MemCardProgressPrimList = arg0;
}

void Menu_DestroyMemCardProgressWidget(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x27));
}

void MemCard_DrawProgress(void) {
    int i;
    int count;

    Draw_OffsetCursor(0, 10);
    Draw_FlushPrimList(g_MemCardProgressPrimList);
    if (g_MemCardProgressPrimList != 0x40) {
        count = MemCard_GetActiveProgressBlocks();
        i = 0;
        Draw_OffsetCursor(0x10, 0x14);
        while (i < 8) {
            if (i < count) {
                Draw_AllocSprite(0x83);
            } else {
                Draw_AllocSprite(0x82);
            }
            i++;
            Draw_OffsetCursor(0x10, 0);
        }
    }
}

int Menu_MemCardProgressInputHandler(void) {
    return 1;
}

void Menu_StepItemGrid2(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *panel;
    MenuWidgetNode *option_node;
    u8 *label;
    int suffix_id;
    MenuConfirmCallback callback;
    int mode;
    int width;
    int height;

    if (g_McDialogMode != 0) {
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x2A) != 0) {
            return;
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, 0x24);
        label = Text_LookupTable4(MemCard_GetActivePort() + 0x47);
        {
            MenuWidgetNode *result = MenuWidget_CreateSimpleNode(0x2A, node, 0, 1);
            mode = 0x2A;
            node = result;
        }
        option_node = MenuWidget_CreateNode(mode, node, node);
        node->draw = Menu_DrawItemLabel;
        panel = node;
        panel->update = Menu_ConfirmDialogHandler;
        option_node->draw = Menu_DrawActionOptionList;
        g_MenuActionTextBase = 0x6C;
        MenuWidget_SaveAndSetCurrentNode(option_node);

        {
            int enabled = 1;
            int active_list;
            suffix_id = 0x4A;
            active_list = g_MenuActiveItemList;
            callback = Menu_HandleMemCardWriteOrError;
            option_node->cursor_x = enabled;
            Inv_SelectActiveList(active_list);
        }

        if (label != 0) {
            Text_CopyFFTerminatedBytes(&g_MenuItemLabelBuffer[0x40], label);
        } else {
            g_MenuItemLabelBuffer[0x40] = 0xFF;
        }
        {
            u8 *text_buf;
            Text_AppendFFTerminatedBytes(&g_MenuItemLabelBuffer[0x40], Text_LookupTable4(0x49));
            g_MenuItemPrimListHandles[1] = suffix_id;
            /* Recompute the buffer address at each measurement. */
            asm volatile("" : "=r"(text_buf) : "0"(&g_MenuItemLabelBuffer[0x40]));
            width = Draw_MeasureTextWidth(text_buf);
            if (width < 0x78) {
                width = 0x78;
            } else {
                asm volatile("" : "=r"(text_buf) : "0"(&g_MenuItemLabelBuffer[0x40]));
                width = Draw_MeasureTextWidth(text_buf);
            }
        }

        panel->grid_width = width + 0x14;
        panel->visible_rows = 0x42;
        panel->x = (0x12C - width) >> 1;
        option_node->x = (panel->grid_width - 0x80) >> 1;
        height = panel->visible_rows;
        g_MenuConfirmCallback = callback;
        option_node->y = height - 0x14;
        return;
    }

    if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
        Menu_CreateNotificationDialog(MemCard_GetActivePort() + 0x47, 0x49);
        g_MenuDeferredCallback = MemCard_ClearActivePrompt;
    }
}

void Menu_NavToSaveConfirmDialog(void) {
    MenuWidget_NavScrollTo(0x2A);
}

s32 Inv_GetPackedListCursor(void);
void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_OpenRenameScreen(s32 arg0);

void Menu_OpenSaveLoadEntryPoint(s32 arg0) {
    MenuInput_InitQueue();
    if ((MenuWidget_FindByModeAndSelectedBase(1, 0xD) == NULL) && (MenuWidget_FindByModeAndSelectedBase(1, 0x17) == NULL)) {
        if (arg0 != 0) {
            if (Inv_GetPackedListCursor() != 0) {
                Menu_StepInventoryRoot(0, -2, -1);
            } else {
                Menu_RequestErrorSound();
                return;
            }
        } else {
            Menu_OpenRenameScreen(-1);
        }
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
            MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = Menu_DrawContextHelpText;
        }
    }
}

void Menu_DrawSaveMetadataPreview(void);

void Menu_OpenRenameScreen(s32 arg0) {
    u8 *var_v1;
    s32 temp_v0_5;
    ItemDataRecord *record;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *var_a3;

    temp_v0 = MenuWidget_CreateSimpleNode(0x17, 0, 0, 0);
    (temp_v0)->update = Menu_StepNameEntryAlt;
    temp_v0_5 = Save_GetMetadataWindowIndex();
    var_v1 = g_MenuRenamePanelPointsWindow0;
    if (temp_v0_5 != 0) {
        var_v1 = g_MenuRenamePanelPointsWindow1;
    }
    temp_v0->appearance.gradientPoints = var_v1;
    if (Save_GetMetadataWindowIndex() != 0) {
        var_a3 = MenuWidget_CreateNode(0x18, temp_v0, temp_v0);
        (var_a3)->draw = &Menu_DrawMemCardSlot2List;
        (var_a3)->y = ((var_a3)->y - 0x34);
        var_a3 = MenuWidget_CreateNode(0x19, temp_v0, temp_v0);
        (var_a3)->draw = &Menu_DrawMemCardSlot3List;
        (var_a3)->y = ((var_a3)->y - 0x34);
    } else {
        (MenuWidget_CreateNode(0x17, temp_v0, temp_v0))->draw = &Menu_DrawMemCardSlot1List;
        (MenuWidget_CreateNode(0x18, temp_v0, temp_v0))->draw = &Menu_DrawMemCardSlot2List;
        var_a3 = MenuWidget_CreateNode(0x19, temp_v0, temp_v0);
        (var_a3)->draw = &Menu_DrawMemCardSlot3List;
    }
    (var_a3)->cursor_x = -1;
    temp_v0 = MenuWidget_CreateSimpleNode(0x11, MenuWidget_FindByModeAndSelectedBase(2, 0x17), 0, 0);
    var_a3 = MenuWidget_CreateNode(0x11, temp_v0, temp_v0);
    (temp_v0)->update = Menu_StepNameEntry;
    (var_a3)->draw = &Menu_DrawItemIconList;
    (var_a3)->cursor_x = 0;
    (var_a3)->cursor_y = 0;
    MenuWidget_SetCurrentNode(var_a3);
    temp_v0 = MenuWidget_CreateSimpleNode(0x1A, 0, 0, 0);
    (temp_v0)->draw = &Menu_DrawSaveMetadataPreview;
    record = Inv_LookupActiveListData(arg0);
    g_MenuRenameTargetRecord = record;
    if (record == 0) {
        temp_v0->appearance.gradientPoints = g_MenuRenamePreviewPanelPoints;
    }
    Save_SelectMetadataWindow(g_MenuRenameTargetRecord);
    Save_LoadMetadataWindowText();
    Inv_ClearEquipFlagForKind(g_MenuRenameTargetRecord);
    g_GameState.flags |= 0x8000;
}

void Menu_DrawSaveMetadataPreview(void) {
    u8 *text;
    s32 remaining;
    s32 lastSlot;
    s32 frame_pad[2];

    (void) frame_pad;
    if (g_MenuRenameTargetRecord != NULL) {
        Draw_OffsetCursor(0x1C, 0xC);
        Menu_CursorRenderData(g_MenuRenameTargetRecord);
        text = g_CursorRenderMetadataWindows[0].text;
        if (g_MenuRenameTargetRecord->kind == ITEM_KIND_ARMOR) {
            text = g_CursorRenderMetadataWindows[1].text;
        }
        Draw_OffsetCursor(Draw_MeasureTextWidth(text) + 0x14, 0xC);
    } else {
        Draw_OffsetCursor(2, 2);
        Draw_AllocSprite(0x47);
        Draw_OffsetCursor(0x3C, 8);
        Draw_PrintRawText(Save_GetActiveMetadataBuffer());
        Draw_OffsetCursor(Draw_MeasureTextWidth(Save_GetActiveMetadataBuffer()), 0xC);
    }
    remaining = Save_GetMetadataRemainingChars();
    lastSlot = remaining - 1;
    if (remaining != 0) {
        do {
            Draw_EmitWipeBarRect(12, 4, 0);
            Draw_OffsetCursor(0xE, 0);
            lastSlot -= 1;
        } while (lastSlot != -1);
    }
}

/* Name entry screen input: the two page handlers share one object
 * (contiguous at 0x8004E074, private data and helpers).
 *
 * The game-state flags are cleared through their scalar symbol g_GameStateFlagsWord,
 * which keeps retail's absolute lui/lw and lui/sw under cc1 -G8. */

int Menu_StepNameEntry(MenuWidgetNode *parent, unsigned int flags)
{
    int row, index, blank;
    MenuWidgetNode *page, *node;
    node = MenuWidget_GetChild(parent, 0);
    if (flags & 0x8000) {
        index = Save_GetMetadataWindowIndex();
        node->cursor_x = -1;
        row = node->cursor_y + index * 3;
        if (row >= 0) {
            if (row < 3) {
                node = MenuWidget_FindByModeAndSelectedBase(2, 23);
                node->cursor_y = row;
            } else if (row < 5) {
                node = MenuWidget_FindByModeAndSelectedBase(2, 24);
                node->cursor_y = row - 3;
            } else {
                node = MenuWidget_FindByModeAndSelectedBase(2, 25);
                node->cursor_y = 0;
            }
        } else {
            node = MenuWidget_FindByModeAndSelectedBase(2, 25);
            node->cursor_y = 0;
        }
        node->cursor_x = 0;
        MenuWidget_SetCurrentNode(node);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        unsigned char *text;
        page = MenuWidget_FindByModeAndSelectedBase(2, 23);
        index = 0x75;
        if (page)
            index = page->cursor_y + 0x73;
        text = Text_LookupTable4(index);
        g_MenuItemIconGlyphTable = text;
        Save_AppendMetadataChar(text[MenuWidget_GridCellIndex(node)]);
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        if (Save_DeleteMetadataChar())
            Menu_PlayCancelSound();
        else
            Menu_PlayErrorSound();
    } else if (flags & 0x800) {
        if (node->selected_base != 25) {
            node->cursor_x = -1;
            page = MenuWidget_FindByModeAndSelectedBase(2, 25);
            page->cursor_x = 0;
            MenuWidget_SetCurrentNode(page);
        } else {
            unsigned char *text = Save_GetMetadataCursor();
            unsigned char *end;
            blank = 1;
            for (end = text; *end != 0xFF;)
                blank &= *end++ == 0x0F;
            if (end > text && !blank) {
                Inv_SetActiveList(9, 0);
                if (!g_MenuRenameTargetRecord)
                    Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
                g_GameStateFlagsWord &= ~0x8000;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
        }
    }
    return 1;
}

#define MENU_INPUT_BACK 0x800
#define MENU_INPUT_UP 0x1000
#define MENU_INPUT_RIGHT 0x2000
#define MENU_INPUT_DOWN 0x4000
#define MENU_INPUT_CONFIRM 0x10000

#define MODE_NAME_WINDOW_0 0x17
#define MODE_NAME_WINDOW_1 0x18
#define MODE_NAME_WINDOW_2 0x19
#define MODE_NAME_GRID 0x11

int Menu_StepNameEntryAlt(MenuWidgetNode *parent, unsigned int input) {
    MenuWidgetNode *node;
    MenuWidgetNode *target;
    MenuWidgetNode *move_node;
    int window_index;
    int selected_offset;
    int next_cursor_y;

    (void)parent;
    node = MenuWidget_GetCurrentNode();

    if (input & MENU_INPUT_CONFIRM) {
        int mode = node->selected_base;
        switch (mode) {
        case MODE_NAME_WINDOW_0:
            input |= MENU_INPUT_RIGHT;
            break;
        case MODE_NAME_WINDOW_1:
            if (node->cursor_y != 0) {
                Save_LoadMetadataWindowText();
                Menu_PlayConfirmSound();
            } else {
                if (Save_DeleteMetadataChar() != 0) {
                    Menu_PlayCancelSound();
                } else {
                    Menu_PlayErrorSound();
                }
            }
            break;
        case MODE_NAME_WINDOW_2: {
            unsigned char *text = Save_GetMetadataCursor();
            unsigned char *end;
            int blank = 1;

            for (end = text; *end != 0xFF;) {
                blank &= (*end++ == 0x0F);
            }

            if (end > text && !blank) {
                Inv_SetActiveList(9, 0);
                if (g_MenuRenameTargetRecord == 0) {
                    Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
                }
                g_GameStateFlagsWord &= 0xFFFF7FFF;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
    }
    }

    {
    if (input & MENU_INPUT_RIGHT) {
        int mode;
        int cursor_y;
        window_index = Save_GetMetadataWindowIndex();
        selected_offset = (window_index << 1) + window_index;
        mode = node->selected_base;
        cursor_y = node->cursor_y;

        if (mode != MODE_NAME_WINDOW_0) {
            int offset;
            if (mode == MODE_NAME_WINDOW_1) {
                offset = selected_offset - 3;
                next_cursor_y = cursor_y - offset;
            } else {
                offset = selected_offset - 5;
                next_cursor_y = cursor_y - offset;
            }
        } else {
            next_cursor_y = cursor_y - selected_offset;
        }

        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, MODE_NAME_GRID);
        node->cursor_x = 0;
        if ((next_cursor_y < 5 - selected_offset) || (node->cursor_y < 6 - selected_offset)) {
            node->cursor_y = next_cursor_y;
        }
        move_node = node;
    } else if (input & MENU_INPUT_UP) {
        int mode;
        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        window_index = Save_GetMetadataWindowIndex();
        {
            int current = node->selected_base;
            mode = MODE_NAME_WINDOW_2;
            if (window_index + MODE_NAME_WINDOW_0 < current) {
                mode = current - 1;
            }
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, mode);
        node->cursor_x = 0;
        node->cursor_y = node->y_limit - 1;
        move_node = node;
    } else if (input & MENU_INPUT_DOWN) {
        int mode;
        int current;
        if (node->selected_base != MODE_NAME_WINDOW_0) {
            node->cursor_x = -1;
        }

        current = node->selected_base;
        if (current < MODE_NAME_WINDOW_2) {
            mode = current + 1;
        } else {
            mode = Save_GetMetadataWindowIndex() + MODE_NAME_WINDOW_0;
        }

        node = MenuWidget_FindByModeAndSelectedBase(2, mode);
        node->cursor_x = 0;
        node->cursor_y = 0;
        move_node = node;
    } else {
        goto name_input_back;
    }

    MenuWidget_SetCurrentNode(move_node);
    Menu_PlayMoveSound();
    return 1;
    }
name_input_back:

    if (input & MENU_INPUT_BACK) {
        if (node->selected_base != MODE_NAME_WINDOW_2) {
            node->cursor_x = -1;
            target = MenuWidget_FindByModeAndSelectedBase(2, MODE_NAME_WINDOW_2);
            target->cursor_x = 0;
            MenuWidget_SetCurrentNode(target);
            return 1;
        }

        {
            unsigned char *text = Save_GetMetadataCursor();
            unsigned char *end;
            int blank = 1;

            for (end = text; *end != 0xFF;) {
                blank &= (*end++ == 0x0F);
            }

            if (end > text && !blank) {
                Inv_SetActiveList(9, 0);
                if (g_MenuRenameTargetRecord == 0) {
                    Menu_CopyPromptCodes((unsigned char *)Save_GetActiveMetadataBuffer());
                }
                g_GameStateFlagsWord &= 0xFFFF7FFF;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
        }
    }

    return 1;
}
