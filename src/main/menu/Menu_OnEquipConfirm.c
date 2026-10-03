/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_inventory.h"
#include "pe1/inventory.h"
#include "pe1/menu_dialog.h"

void Menu_OnEquipConfirm(MenuWidgetNode *unused, int confirmed)
{
    MenuWidgetNode *node;
    MenuWidgetNode *next;
    if (confirmed) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 7);
        if (D_8009CF18) {
            Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(node));
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : next);
            MenuWidget_NavScrollTo(0x35);
        } else if (!Inv_GetSlotItemData(MenuWidget_GridCellIndex(node))) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : next);
            MenuWidget_NavScrollTo(0x35);
        }
        Inv_BuildFilteredPackedList(D_8009CF18 ? 0x1FE : 0x200);
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node == MenuWidget_GetCurrentNode()) {
            node->cursor_y = 0;
            node->cursor_x = 0;
        }
        if (D_8009CF18) {
            MenuInput_SetPollingPaused(1);
            D_8009CFB0 = 1;
        }
    }
}

/* Confirmation input handling shares the callback's equip-selection flow. */
extern int D_8009CF0C;
extern int D_8009CF10;
extern int D_8009CF14;
extern int D_8009CF1C;
extern int D_8009CFA0;
extern int D_8009CFA8;
extern int D_800A1888[];
extern int D_800A188C[];
extern int D_800A1890[];
extern int D_800A1894[];
extern u8 D_800A1980[];

int Menu_GetBattleEquipMode(void);
int Str_LookupTable4(int index);
void Util_CopyFFTerminatedBytes(void *dst, void *src);
void Util_AppendFFTerminatedBytes(void *dst, void *src);
int Draw_MeasureTextWidth(void *text);
void Menu_StepInventoryRoot(int flags, int arg1, int arg2);
void Menu_ConfigureScreen(void);

int Menu_StepEquipConfirm(MenuWidgetNode *node, int input) {
    register MenuWidgetNode *child asm("$16");
    MenuWidgetNode *parent;
    MenuWidgetNode *option_node;
    int item;
    register int handled asm("$18");
    int allowed;
    int first_width;
    int width;
    register MenuWidgetNode *second_parent asm("$19");
    register u8 *dialog_text asm("$17");
    void (*callback)(MenuWidgetNode *, int);

    handled = 0;
    child = MenuWidget_GetChild(node, 0);

    if ((input & 0x10000) == 0) goto no_confirm;
    {
        item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(child));
        if (D_8009CF1C != 0) {
            Inv_SelectActiveList(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 0x36)) == 1);

            allowed = 0;
            if ((D_800A1888[0] + D_800A188C[0] + D_800A1890[0] + D_800A1894[0]) != 0) {
                unsigned mask = Inv_LookupActiveListData(item)->flags & 0x40;
                allowed = mask < 1U;
            }

            if (allowed == 0) goto error;

            Inv_RememberSelection(1, item);
            MenuWidget_NavScrollTo(0x36);
            Menu_ConfigureScreen();
            goto confirm;
        }

        goto reserved;
confirm:
        handled = 1;
        Menu_PlayConfirmSound();
        goto done;
reserved:
        if (Inv_IsAyaInventorySlotReserved(item) != 0) goto error;
        if (Menu_GetBattleEquipMode() == 0) goto simple_confirm;

        {
            MenuWidgetNode *result = MenuWidget_CreateSimpleNode(0x29, child, 0, 1);
            register int mode asm("$4") = 0x29;
            /* Keep the mode argument ready before copying the returned node. */
            asm volatile("" : : "r"(mode));
            parent = result;
            option_node = MenuWidget_CreateNode(mode, parent, parent);
        }
        parent->draw = Menu_DrawItemLabel;
        parent->update = Menu_ConfirmDialogHandler;
        option_node->draw = Menu_DrawActionOptionList;
        D_8009CF14 = 5;
        MenuWidget_SetCurrentNode(option_node);
        Inv_SelectActiveList(D_8009CF10);

        dialog_text = D_800A1980;
        dialog_text[0] = 0xFF;
        Util_AppendFFTerminatedBytes(dialog_text, (void *)Str_LookupTable4(0));
        D_8009CFA0 = 0;
        callback = Menu_OnEquipConfirm;
        first_width = Draw_MeasureTextWidth(dialog_text);
        if (first_width < 0x78) {
            first_width = 0x78;
        } else {
            first_width = Draw_MeasureTextWidth(dialog_text);
        }

        parent->grid_width = first_width + 0x14;
        parent->visible_rows = 0x32;
        parent->x = (0x12C - first_width) >> 1;
        option_node->x = (parent->grid_width - 0x80) >> 1;
        {
            int height = parent->visible_rows;
            D_8009CFA8 = (int)callback;
            option_node->y = height - 0x14;
        }

        Util_CopyFFTerminatedBytes(D_800A1980, (void *)Str_LookupTable4(0x71));
        Util_AppendFFTerminatedBytes(D_800A1980, Inv_LookupActiveListDisplayData(item));
        if (Draw_MeasureTextWidth(D_800A1980) >= 0x78) {
            width = Draw_MeasureTextWidth(D_800A1980);
        } else {
            width = 0x78;
        }

        second_parent = MenuWidget_FindByModeAndSelectedBase(1, 0x29);
        /* Copy the returned node before preparing the next lookup. */
        asm volatile("" : : "r"(second_parent));
        second_parent->grid_width = width + 0x14;
        second_parent->x = (0x12C - width) >> 1;
        option_node = MenuWidget_FindByModeAndSelectedBase(2, 0x29);
        option_node->x = (second_parent->grid_width - 0x80) >> 1;
        goto confirm;
simple_confirm:
        Menu_OnEquipConfirm(node, 1);
        goto confirm;
    }
error:
    handled = 1;
    Menu_PlayErrorSound();
    goto done;
no_confirm:
    if ((input & 0x1040) == 0) goto done;

    if (D_8009CF0C != 0) {
        MenuWidget_NavScrollTo(0x35);
        child->cursor_x = -1;
        child->scroll_y = 0;
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x36));
        goto cancel;
    }

    if ((input & 0x40) == 0) {
        handled = 1;
        goto done;
    }

    MenuWidget_NavScrollTo(0x35);
    if (D_8009CF1C != 0) {
        MenuWidget_DestroyNode(node);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
        Menu_StepInventoryRoot(0x33E, -1, -1);
        goto cancel;
    }

    child->cursor_x = -1;
    parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
    option_node = MenuWidget_GetCurrentNode();
    if (parent == option_node) {
        parent->cursor_x = ((unsigned)parent->has_scroll < 1U);
        MenuWidget_SetCurrentNode(parent);
    } else {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 5));
    }

cancel:
    handled = 1;
    Menu_PlayCancelSound();
done:
    return handled;
}
