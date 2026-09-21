#include "pe1/menu_item_rows.h"
#include "pe1/battle_modifiers.h"

void Menu_DrawWeaponList(MenuWidgetNode *node) {
    Inv_SelectActiveList(0);
    MenuWidget_DrawList(node, Menu_DrawInventoryItemRow);
}

void Menu_DrawArmorList(MenuWidgetNode *node) {
    Inv_SelectActiveList(1);
    MenuWidget_DrawList(node, Menu_DrawArmorItemRow);
}
