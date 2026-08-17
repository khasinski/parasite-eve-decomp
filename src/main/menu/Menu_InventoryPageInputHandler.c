/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_inventory.h"

int Menu_InventoryPageInputHandler(MenuWidgetNode *root, u32 flags) {
    register MenuWidgetNode *child asm("$16");
    register MenuWidgetNode *node asm("$17");
    register int handled asm("$19");

    handled = 0;
    child = MenuWidget_GetChild(root, 0);
    node = MenuWidget_FindByModeAndSelectedBase(2, 0xB);

    if (node != 0) {
        handled = Menu_InventoryNavigate(child, node, flags);
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
        register MenuWidgetNode *node2 asm("$16");
        child->cursor_x = -1;
        if (node == 0) {
            node2 = MenuWidget_FindByModeAndSelectedBase(2, 5);
        } else {
            node2 = MenuWidget_FindByModeAndSelectedBase(2, 0x1B);
        }

        if (node2 != 0) {
            node2->cursor_x = 0;
            node2->cursor_y = node2->y_limit - 1;
            MenuWidget_SetCurrentNode(node2);
        }
        Menu_PlayMoveSound();
    }

    return 1;
}
