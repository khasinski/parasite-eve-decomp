#ifndef PE1_MENU_ITEM_LIST_INPUT_H
#define PE1_MENU_ITEM_LIST_INPUT_H

/* Declarations used only by Menu_ItemListInputHandler. */
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/inventory.h"

int Menu_ItemListInputHandler(MenuWidgetNode *node, unsigned int flags);
int Menu_InventoryPageInputHandler(MenuWidgetNode *root, unsigned int flags);
int Menu_StepSkillScreen(void *node, int flags);
void Menu_DrawUsableItemActionList();
void Menu_DrawSoundTestList(void *node);
void Menu_DrawItemListInvPanel(int node);
void Menu_SetupSkillSubmenu(int node);
void Menu_StepSkillList(MenuWidgetNode *panel, int refresh);
void Menu_OpenRenameScreen(int item);
void Menu_ReopenEquipScreen(void);
void Menu_ItemUseAction(int node, int confirmed);

/* Bonus point allocation view is open. */
extern int D_8009CEF8;
/* Item-use confirmation dialog: selected option and confirm callback. */
extern int D_8009CFA0;
extern void (*D_8009CFA8)(int node, int confirmed);
/* Pending stat bonus: item index (negative when none), stat and amount. */
extern int D_8009CFD4;
/* Copy of the item record being modified. */
extern ItemDataRecord D_800A1960;
/* Confirmation dialog text buffer. */
extern u8 D_800A1980[];

#endif
