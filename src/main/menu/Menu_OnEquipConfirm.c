/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

extern int D_8009CF18;
extern int D_8009CFB0;

extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
extern int MenuWidget_GridCellIndex(MenuWidgetNode *node);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern MenuWidgetNode *MenuWidget_GetCurrentNode(void);
extern void MenuWidget_NavScrollTo(int selected_base);
extern void Menu_OpenItemUsePanelAtIndex(int index);
extern int Inv_GetSlotItemData(int index);
extern void Menu_CreateNotificationDialog(int line0_id, int line1_id);
extern void Inv_BuildFilteredPackedList(int mask);
extern void MenuInput_SetPollingPaused(int paused);

void Menu_OnEquipConfirm(MenuWidgetNode *node, int confirmed) {
    MenuWidgetNode *selected;
    MenuWidgetNode *target;

    if (confirmed == 0) {
        return;
    }

    selected = MenuWidget_FindByModeAndSelectedBase(2, 7);

    if (D_8009CF18 != 0) {
        Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(selected));
        selected->cursor_x = -1;
        target = MenuWidget_FindByModeAndSelectedBase(2, 5);
        if (target->cursor_x < 0) {
            target = MenuWidget_FindByModeAndSelectedBase(2, 6);
        }
        MenuWidget_SetCurrentNode(target);
        MenuWidget_NavScrollTo(0x35);
    } else {
        if (Inv_GetSlotItemData(MenuWidget_GridCellIndex(selected)) == 0) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            selected->cursor_x = -1;
            target = MenuWidget_FindByModeAndSelectedBase(2, 5);
            if (target->cursor_x < 0) {
                target = MenuWidget_FindByModeAndSelectedBase(2, 6);
            }
            MenuWidget_SetCurrentNode(target);
            MenuWidget_NavScrollTo(0x35);
        }
    }

    if (D_8009CF18 != 0) {
        Inv_BuildFilteredPackedList(0x1FE);
    } else {
        Inv_BuildFilteredPackedList(0x200);
    }

    selected = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (selected == MenuWidget_GetCurrentNode()) {
        selected->cursor_y = 0;
        selected->cursor_x = 0;
    }

    if (D_8009CF18 != 0) {
        MenuInput_SetPollingPaused(1);
        D_8009CFB0 = 1;
    }
}
