/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --use-comm-section */

#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, int arg1, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Draw_SetPrimCallback(MenuWidgetNode *node, int arg1);
void Menu_MainMenuInputHandler(void);
void Menu_DrawMainMenuList(void);

void Menu_CreateMainMenuView(int arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x20, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x20, parent, parent);
    parent->update = Menu_MainMenuInputHandler;
    child->field_30 = Menu_DrawMainMenuList;
    MenuWidget_SetCurrentNode(child);
    Draw_SetPrimCallback(child, 4);
}
