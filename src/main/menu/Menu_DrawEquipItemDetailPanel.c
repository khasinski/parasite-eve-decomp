/* CC1_FLAGS: -G8 */
#include "pe1/menu_item_record.h"
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;
ItemDataRecord *g_MenuActionItemData;

void Menu_DrawActionCodeItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawEquipItemDetailPanel(int arg0) {
    g_MenuActiveWidget = arg0;
    g_MenuActionItemData = g_MenuSelectedItemData;
    MenuWidget_DrawList(arg0, Menu_DrawActionCodeItem);
}
