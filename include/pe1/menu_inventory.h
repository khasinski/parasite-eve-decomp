#ifndef PE1_MENU_INVENTORY_H
#define PE1_MENU_INVENTORY_H

#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index);
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode,
                                                     int selectedBase);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void MenuWidget_DestroyNode(MenuWidgetNode *node);
void MenuWidget_NavScrollTo(int selectedBase);
int Menu_InventoryNavigate(MenuWidgetNode *current, MenuWidgetNode *node,
                           u32 flags);
void Menu_CreateBonusPointAllocationView(void);
void Menu_PlayCancelSound(void);
void Menu_PlayMoveSound(void);

#endif
