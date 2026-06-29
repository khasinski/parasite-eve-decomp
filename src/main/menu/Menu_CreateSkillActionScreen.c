/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --use-comm-section */

#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, int arg1, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Draw_SetPrimCallback(MenuWidgetNode *node, int arg1);
void Item_PackParasiteSpells(void);
int Inv_GetPackedListCount(void);
void Menu_SkillActionHandler(void);
void Menu_DrawEquipModList(void);
void Menu_GetItemSlotAvailability(void);

void Menu_CreateSkillActionScreen(int arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(8, arg0, 0, 0);
    child = MenuWidget_CreateNode(8, parent, parent);
    parent->update = Menu_SkillActionHandler;
    child->field_30 = Menu_DrawEquipModList;
    child->field_8C = (int)Menu_GetItemSlotAvailability;
    MenuWidget_SetCurrentNode(child);
    Item_PackParasiteSpells();
    Draw_SetPrimCallback(child, Inv_GetPackedListCount());
}
