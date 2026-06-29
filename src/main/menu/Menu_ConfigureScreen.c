/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

extern void MenuWidget_NavScrollTo(int arg0);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
extern void MenuWidget_SetCursorY(MenuWidgetNode *node);
extern void Menu_CreateItemDetailView(MenuWidgetNode *node);
extern void Menu_CreateEquipStatsPanel(MenuWidgetNode *node);
extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int arg1);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);

void Menu_ConfigureScreen(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *other;
    int value;

    MenuWidget_NavScrollTo(0x35);

    node = MenuWidget_FindByModeAndSelectedBase(1, 5);
    node->grid_width = 0x80;
    node = MenuWidget_FindByModeAndSelectedBase(1, 6);
    node->grid_width = 0x80;
    node = MenuWidget_FindByModeAndSelectedBase(2, 6);
    node->field_3C = 0x3E;

    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 7));

    node = MenuWidget_FindByModeAndSelectedBase(2, 0);
    if (node == 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
    }

    Menu_CreateItemDetailView(node);
    Menu_CreateEquipStatsPanel(node);

    node = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 5), 1);
    if (node != 0) {
        MenuWidget_SetCurrentNode(node);
        node->cursor_x = 0;
    }

    node = MenuWidget_FindByModeAndSelectedBase(2, 0x1B);
    other = MenuWidget_FindByModeAndSelectedBase(2, 0x1C);
    node->field_7C = other;
    other->field_78 = node;

    node = MenuWidget_FindByModeAndSelectedBase(2, 5);
    if (node != 0) {
        value = -1;
        node->cursor_x = value;
    }
}
