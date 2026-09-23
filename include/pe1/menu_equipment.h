#ifndef PE1_MENU_EQUIPMENT_H
#define PE1_MENU_EQUIPMENT_H

#include "pe1/menu_dialog.h"
#include "pe1/inventory_slots.h"

void Menu_StepSaveLoadScreen(void);
int Menu_SaveLoadNavigate(MenuWidgetNode *node, unsigned int flags);
int Menu_InventoryInputHandler(MenuWidgetNode *node, unsigned int flags);
void Menu_DrawArmorListContainer(int node);
void Menu_DrawArmorSelectionList(int node);
void Menu_DrawSelectableEquipSlotList(int node);
void Menu_RebuildSelectableMask(void);
int Inv_TransferItem(int arg0, int arg1, int arg2, int arg3);
void MenuWidget_ClearColumnLayout(void *node);
int Menu_GetBattleCount(void);
void MenuWidget_ClampCursor(MenuWidgetNode *node, int x, int y);
extern int D_8009CF94, D_8009CF8C, D_8009CF00;

PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, itemAction) == 0x84,
                  menu_widget_item_action_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, refreshItems) == 0x88,
                  menu_widget_refresh_items_offset);

void Menu_OpenEquipScreen(int mode);
int Menu_StepInventoryCategory(MenuWidgetNode *node, unsigned int flags);
void Menu_DrawEquipListContainer(void *node);
void Menu_DrawContextHelpText(void);
void Menu_SetBattleEquipMode(int mode);
extern int D_8009CF0C, D_8009CEFC;

int Menu_EquipOptionsInputHandler(int node, int flags);
void Menu_DrawEquipOptionsList(int node);
void MenuWidget_SetCursorY(MenuWidgetNode *node);
void Menu_OpenInventoryScreen(void);
extern int D_8009CFB8, D_8009CF1C;
/* Equipment selection state and stat preview. */
extern int D_8009CF30, D_8009CF34, D_8009CF38;
extern int D_8009CFC4, D_8009CFC8, D_8009CFCC;
extern int g_StatBaseTable[4];

void Menu_StepEquipSlotSelect(void);
void Menu_DrawEquipStatsDelta(ItemDataRecord *preview);
void Menu_StepItemGridScroll(ItemDataRecord *item, ItemDataRecord *other);

int Menu_StepEquipScreen(MenuWidgetNode *node, unsigned int flags);
void Menu_OnInventoryItemConfirm(int index);
extern int D_8009CF10, D_8009CF04, D_8009CF08, D_8009CDA8;
/* Three action IDs per menu configuration, indexed by the selected child cell. */
extern int D_80092234[][3];

#endif
