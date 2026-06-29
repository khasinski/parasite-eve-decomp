/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, int arg1, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
void Menu_DrawEquipStats(void);
void Menu_EquipStatsInputHandler(void);
void Menu_DrawItemListInvPanel(void);

void Menu_CreateEquipStatsPanel(int arg0) {
    MenuWidgetNode *node;
    MenuWidgetNode *child;

    node = MenuWidget_CreateSimpleNode(0xA, arg0, 0, 0);
    node->field_30 = Menu_DrawEquipStats;
    node->update = Menu_EquipStatsInputHandler;

    child = MenuWidget_CreateNode(0x1C, node, node);
    child->field_30 = Menu_DrawItemListInvPanel;
    child->cursor_x = -1;
}
