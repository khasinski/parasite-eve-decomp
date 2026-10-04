/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_item_list_input.h"
#include "pe1/text.h"

/* Leaves the item list: scrolls the inventory panels back and returns the
 * focus to the equipment slots or the category tabs. */
static inline void Menu_CloseItemList(void)
{
    MenuWidget_NavScrollTo(0xF);
    MenuWidget_NavScrollTo(0xB);
    MenuWidget_NavScrollTo(0xD);
    MenuWidget_NavScrollTo(0x18);
    MenuWidget_NavScrollTo(0x30);
    if (D_8009CF0C != 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
    } else {
        if (MenuWidget_GetCurrentNode() != 0)
            MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    }
    D_8009CF34 = 0;
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
    root->update = (void (*)())Menu_InventoryPageInputHandler;
    root->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    node->layout_flags |= 0x80;
    if (D_8009CF1C != 0)
        MenuWidget_ClearColumnLayout(node);
    else if (D_8009CF18 == 0)
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
    root->update = (void (*)())Menu_StepSkillScreen;
    node->draw = Menu_DrawSoundTestList;
    if (D_8009CF1C != 0)
        MenuWidget_ClearColumnLayout(node);
    else if (D_8009CF18 == 0)
        MenuWidget_SetColumnLayout(node, 0x12);
    if (node->cursor_x >= 0)
        MenuWidget_SetCurrentNode(node);
    root->disabled = 1;
    node = MenuWidget_CreateNode(0x1B, root, root);
    node->draw = (void (*)())Menu_DrawItemListInvPanel;
    MenuWidget_ClearColumnLayout(node);
}

/* Whether the selected item can go into an equipment slot that has room. */
static inline int Menu_CanEquipSelection(int selection)
{
    int usable;

    usable = 0;
    if (D_800A1888[0] + D_800A188C[0] + D_800A1890[0] + D_800A1894[0] != 0
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
        if (D_8009CFC4 >= 0) {
            switch (D_8009CFC8) {
            case 0:
                data->bonusStats[0] = data->bonusStats[0] + D_8009CFCC >= 1000
                                          ? 999 : data->bonusStats[0] + D_8009CFCC;
                break;
            case 1:
                data->bonusStats[1] = data->bonusStats[1] + D_8009CFCC >= 1000
                                          ? 999 : data->bonusStats[1] + D_8009CFCC;
                break;
            case 2:
                data->bonusStats[2] = data->bonusStats[2] + D_8009CFCC >= 1000
                                          ? 999 : data->bonusStats[2] + D_8009CFCC;
                break;
            }
            Inv_SelectActiveList(D_8009CFD4);
            Inv_RemoveActiveListItem(D_8009CFC4);
            Menu_CloseItemList();
            if (D_8009CF0C == 1) {
                MenuWidget_NavScrollTo(0x36);
                Menu_ReopenEquipScreen();
            }
            Menu_PlayConfirmSound();
            return 1;
        } else if (D_8009CF1C != 0) {
            if (Menu_CanEquipSelection(Inv_RestoreSelection(0)) != 0) {
                Menu_CloseItemList();
                MenuWidget_OffsetPosition(MenuWidget_FindByModeAndSelectedBase(1, 0x36), 0x98, 0);
                D_8009CF18 = data->kind != 9;
                panel = MenuWidget_FindByModeAndSelectedBase(2, 0);
                if (panel == 0)
                    panel = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
                Menu_CreateUsableItemActionView(panel);
                Menu_CreateEquipItemSelectionView(panel);
                Menu_StepSkillList(panel, 1);
                if (D_8009CF0C == 0) {
                    MenuWidgetNode *submenu;

                    submenu = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                    MenuWidget_CreateNode(0x35, submenu, submenu)->draw = (void (*)())Menu_SetupSkillSubmenu;
                }
                /* The list child is no longer needed: reuse it for the equipment node. */
                child = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
                if (child != 0)
                    MenuWidget_SetCurrentNode(child);
                return 1;
            }
        } else if (D_8009CF30 != 0) {
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
            D_800A1960 = *Inv_LookupActiveListData(Inv_RestoreSelection(0));
            Menu_PlayConfirmSound();
            return 1;
        } else if (D_8009CF34 != 0 && Inv_TestSelectionBit(index) != 0) {
            Menu_CloseItemList();
            Menu_OpenRenameScreen(item);
            Menu_PlayConfirmSound();
            return 1;
        } else if (D_8009CF38 != 0) {
            if (Inv_TestSelectionBit(index) != 0) {
                MenuWidgetNode *dialog;
                MenuWidgetNode *options;
                void (*action)(int node, int confirmed);
                int width;

                dialog = MenuWidget_CreateSimpleNode(0x29, child, 0, 1);
                options = MenuWidget_CreateNode(0x29, dialog, dialog);
                dialog->draw = (void (*)())Menu_DrawItemLabel;
                dialog->update = (void (*)())Menu_ConfirmDialogHandler;
                options->draw = (void (*)())Menu_DrawActionOptionList;
                D_8009CF14 = 5;
                MenuWidget_SetCurrentNode(options);
                Inv_SelectActiveList(D_8009CF10);
                D_800A1980[0] = 0xFF;
                Util_AppendFFTerminatedBytes(D_800A1980, Str_LookupTable4(0x61));
                D_8009CFA0 = 0;
                action = Menu_ItemUseAction;
                if (Draw_MeasureTextWidth(D_800A1980) < 0x78)
                    width = 0x78;
                else
                    width = Draw_MeasureTextWidth(D_800A1980);
                dialog->grid_width = width + 0x14;
                dialog->visible_rows = 0x32;
                dialog->x = (300 - width) >> 1;
                options->x = (dialog->grid_width - 0x80) >> 1;
                D_8009CFA8 = action;
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
        } else if (D_8009CF34 != 0 || D_8009CF38 != 0) {
            D_8009CF34 = 0;
            D_8009CF38 = 0;
            Inv_SetActiveList(9, 0);
        } else {
            MenuWidget_NavScrollTo(0x36);
            if (D_8009CF1C != 0) {
                D_8009CF1C = 0;
                Menu_CloseItemList();
                MenuWidget_NavScrollTo(0x2F);
                if (D_8009CF0C == 0) {
                    Menu_CreateBonusPointAllocationView();
                    D_8009CEF8 = 1;
                }
            } else if (D_8009CF30 != 0) {
                func_800490B0();
            } else {
                Menu_CloseItemList();
            }
        }
        Menu_PlayCancelSound();
    }
    return 1;
}
