/* CC1_FLAGS: -g3 */

#include "pe1/menu_widget.h"

extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *arg0, int arg1);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern void Menu_InventoryNavigate(MenuWidgetNode *current, MenuWidgetNode *node, unsigned int flags);
extern void Menu_PlayMoveSound(void);

int Menu_DetailViewInput(MenuWidgetNode *arg0, unsigned int arg1) {
    MenuWidgetNode *node;

    node = MenuWidget_GetChild(arg0, 0);
    Menu_InventoryNavigate(node, MenuWidget_FindByModeAndSelectedBase(2, 6), arg1);

    if (arg1 & 0x1000) {
        node->cursor_x = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x1C);
        node->cursor_x = 0;
        node->cursor_y = node->y_limit - 1;
        MenuWidget_SetCurrentNode(node);
        Menu_PlayMoveSound();
    }

    return 1;
}
