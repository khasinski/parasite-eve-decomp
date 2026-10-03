#ifndef PE1_MENU_CONTEXT_HELP_H
#define PE1_MENU_CONTEXT_HELP_H

/* Context help line drawn under the menu clock by Menu_DrawContextHelpText. */

#include "pe1/menu_inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/text.h"
#include "pe1/draw_state.h"

void Draw_StatePush(void);
void Draw_StatePop(void);
int GameTime_GetCounterSeconds(int counter);
void Draw_PrintTimeValue(int seconds, int showSeconds);
void Draw_PrintRawText(u8 *text);
void Draw_SetTextDimmed(int dimmed);
u8 *Str_LookupTableC(unsigned int index);
u8 *Str_LookupTable10(unsigned int index);
ItemDataRecord *Inv_LookupItemData(int item);
int Inv_GetWayneListItemByIndex(int index);
int Inv_GetPackedListCount(void);
int Inv_GetPackedListItem(int index);
int Inv_RestoreSelection(unsigned int index);
void Inv_SelectActiveList(int list);
int Menu_GetBattleCountEntry(int index);
int MemCard_PollTransferDelay(void);
int MemCard_IsPortPresent(int port);
int Save_GetTitleStyleFlag(void);

/* Menu state shared with the equipment screens (see menu_equipment.h, which
 * declares this function without its panel parameter). */
extern int D_8009CF8C, D_8009CF0C, D_8009CEFC, D_8009CF1C;
/* Bit mask of entries listed on help page 0. */
extern int D_8009CEF0;
/* Equipment record whose modification slots the stat page describes. */
extern ItemDataRecord *D_8009CF20;
/* Memory card screen flags selecting the save prompt help. */
extern int D_8009CF50, D_8009CFF8;
/* Five help text ids indexed by row; row 2 is replaced by entry 4 in
 * equip mode 1. */
extern u8 D_8009234C[];

#endif
