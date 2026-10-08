/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_inventory.h"

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
