#ifndef PE1_MENU_ITEM_RECORD_H
#define PE1_MENU_ITEM_RECORD_H

#include "pe1/inventory.h"

extern ItemDataRecord *g_MenuSelectedItemData;
extern ItemDataRecord *g_MenuActionItemData;
void Menu_CreateItemUsePanel(int itemId);

#endif
