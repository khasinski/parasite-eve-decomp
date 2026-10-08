#ifndef PE1_MENU_INVENTORY_ROOT_H
#define PE1_MENU_INVENTORY_ROOT_H

/* Callbacks and state wired up by Menu_StepInventoryRoot. */

#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/inventory.h"
#include "pe1/aya.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_item_list_input.h"

int Menu_InventoryPageNavHandler(MenuWidgetNode *root, unsigned int flags);
void Menu_DrawBattleCommandList();
void Menu_DrawEquipSelectionList(MenuWidgetNode *node);
void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
int Inv_GetPackedListCount(void);
void Menu_DrawBonusPointSlotValue(void);
void Menu_DrawParasiteAbilityList();
void Menu_InitStateTables(void);
void Menu_DrawItemDetailPanel(MenuWidgetNode *panel);
void Menu_DrawUsableItemActionList();
int Menu_BonusPointCancelHandler(void *node, int flags);
void Menu_DrawEmptyList();
void Menu_StepEquipSlotSelect2(void);
void Menu_SetEquipPanelsCursorY(void);
void Menu_AlignEquipPanels(void);

/* Whether the active list override was selected when the root opened. */
extern int D_8009CFD4;
/* Per-stat preview tables: value, level, sublevel and pending bonus. */
extern int D_800A1898[7];
extern int D_800C0E10[];
extern int D_8009CF80, D_8009CF40, D_8009CF68;

#endif
