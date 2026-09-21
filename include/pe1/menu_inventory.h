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
void Menu_SetSwapReturnFlag(void);
void Menu_PlayMoveSound(void);

extern int D_8009CF18, D_8009CFB0;
int MenuWidget_GridCellIndex(MenuWidgetNode *node);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
void Menu_OpenItemUsePanelAtIndex(int index);
void Menu_OpenSkillSelectionView(void);
void Menu_StepInventoryRoot(int mode, int index, int arg);
void Menu_CreateNotificationDialog(int message, int arg);
void MenuInput_SetPollingPaused(int paused);
void Menu_OnEquipConfirm(int unused, int confirmed);

int Menu_ClampRange(int value);
void Menu_SaveBgInitFade(void);

#endif
