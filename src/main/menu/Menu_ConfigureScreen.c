/* CC1_FLAGS: -g3 */

#include "pe1/menu_widget.h"

void MenuWidget_NavScrollTo(int selected_base);
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_SetCursorY(MenuWidgetNode *ptr);
void Menu_CreateItemDetailView(MenuWidgetNode *arg0);
void Menu_CreateEquipStatsPanel(int arg0);
int MenuWidget_GetChild(int *arg0, int arg1);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);

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
