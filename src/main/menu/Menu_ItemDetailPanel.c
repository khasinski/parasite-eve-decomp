/* CC1_FLAGS: -G8 -fno-expensive-optimizations */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_dialog.h"
#include "pe1/text.h"
#include "common.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/inventory_slots.h"

void Menu_StepItemDetailPanel(void) {
    MenuWidgetNode *parent, *child;
    u8 *label;
    void (*confirm)(void *, int);
    int width;
    Inv_SelectActiveList(g_MenuActiveItemList);
    parent = MenuWidget_GetCurrentNode();
    label = Inv_LookupActiveListDisplayData(g_MenuActiveItemSlot);
    {
        MenuWidgetNode *result = MenuWidget_CreateSimpleNode(0x29, parent, 0, 1);
        register int mode asm("$4") = 0x29;
        /* Keep the mode argument ready before copying the returned node. */
        asm("" : : "r"(result), "r"(mode));
        parent = result;
        child = MenuWidget_CreateNode(mode, parent, parent);
    }
    parent->draw = Menu_DrawItemLabel;
    parent->update = Menu_ConfirmDialogHandler;
    child->draw = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(child);
    child->cursor_x = 1;
    confirm = Menu_OnItemDiscardConfirm;
    Inv_SelectActiveList(g_MenuActiveItemList);
    if (label) Util_CopyFFTerminatedBytes(g_MenuItemLabelBuffer, label);
    else g_MenuItemLabelBuffer[0] = 0xFF;
    Util_AppendFFTerminatedBytes(g_MenuItemLabelBuffer, Str_LookupTable4(4));
    g_MenuItemPrimListHandles[0] = 0;
    width = Draw_MeasureTextWidth(g_MenuItemLabelBuffer) < 120 ? 120 : Draw_MeasureTextWidth(g_MenuItemLabelBuffer);
    parent->grid_width = width + 20;
    parent->visible_rows = 50;
    parent->x = (300 - width) >> 1;
    child->x = (parent->grid_width - 128) >> 1;
    child->y = parent->visible_rows - 20;
    g_MenuConfirmCallback = confirm;
}

extern s32 g_MenuEquipMode;

/* Discard confirmation: on "yes" remove the selected item, close the
 * dialog and refresh the slot counts of the open inventory panels. */
void Menu_OnItemDiscardConfirm(void *arg0, s32 arg1) {
    MenuWidgetNode *dialog = arg0;
    MenuWidgetNode *panel;

    if (arg1 != 0) {
        Inv_SelectActiveList(g_MenuActiveItemList);
        Inv_RemoveActiveListItem(g_MenuActiveItemSlot);
        Inv_RebuildSelectableMask();
        MenuWidget_DestroyNode(dialog->parent->parent);
        panel = MenuWidget_FindByModeAndSelectedBase(2, 1);
        if (panel != 0) {
            Draw_SetPrimCallback(panel, Inv_GetAyaSlotLimit());
        }
        if (g_MenuEquipMode == 2) {
            panel = MenuWidget_FindByModeAndSelectedBase(2, 0x33);
            if (panel != 0) {
                Draw_SetPrimCallback(panel, Inv_TransferItemAlt2(0x3803FE));
            }
        }
    }
}
