#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, int arg1, int arg2, int arg3);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Menu_DrawExpReviewPanel(void);
void Menu_ConfirmExpAllocation(void);
extern int D_800922F4;

void Menu_CreateExpReviewView(void) {
    MenuWidgetNode *node;

    node = MenuWidget_CreateSimpleNode(0x14, 0, 0, 0);
    node->field_30 = Menu_DrawExpReviewPanel;
    node->update = Menu_ConfirmExpAllocation;
    MenuWidget_SetCurrentNode(node);
    node->target_x = (int)&D_800922F4;
}
