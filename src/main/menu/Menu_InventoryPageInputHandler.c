/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

typedef unsigned int u32;

extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern void MenuWidget_DestroyNode(MenuWidgetNode *node);
extern void MenuWidget_NavScrollTo(int selected_base);
extern int Menu_InventoryNavigate(MenuWidgetNode *current, MenuWidgetNode *node, u32 flags);
extern void Menu_CreateBonusPointAllocationView(void);
extern void Menu_PlayCancelSound(void);
extern void Menu_PlayMoveSound(void);

int Menu_InventoryPageInputHandler(MenuWidgetNode *root, u32 flags) {
    MenuWidgetNode *child;
    MenuWidgetNode *node;
    int handled;

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
        child->cursor_x = -1;
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 5);
        } else {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x1B);
        }

        if (node != 0) {
            node->cursor_x = 0;
            node->cursor_y = node->y_limit - 1;
            MenuWidget_SetCurrentNode(node);
        }
        Menu_PlayMoveSound();
    }

    return 1;
}
