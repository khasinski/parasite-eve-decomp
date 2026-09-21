#ifndef PE1_MENU_ITEM_ROWS_H
#define PE1_MENU_ITEM_ROWS_H

#include "pe1/menu_widget.h"

/* The list renderer passes the row index in a0 to its drawing callback. */
void MenuWidget_DrawList(MenuWidgetNode *node, void (*draw)(int index));
void Menu_DrawWeaponList(MenuWidgetNode *node);
void Menu_DrawArmorList(MenuWidgetNode *node);
void Menu_DrawInventoryItemRow(int index);
void Menu_DrawArmorItemRow(int index);
void Menu_DrawArmorItemDetail(int index);
void Sfx_DrawActiveListSlot(int index);
void Draw_SetTextDimmed(int dimmed);
void Draw_DropShadow(void);
void MenuWidget_DrawCenteredTableText(int textId);

#endif
