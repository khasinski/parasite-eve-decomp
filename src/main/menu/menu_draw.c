#include "pe1/inventory.h"
#include "pe1/menu_item_rows.h"

void Menu_DrawInventoryItemRow(int index) {
    int slot = Inv_GetWayneListItemByIndex(index);

    Draw_SetTextDimmed(Inv_TestSelectionBit(slot) == 0);
    Sfx_DrawActiveListSlot(slot);
    if (Inv_IsAyaInventorySlotReserved(slot) != 0) {
        Draw_DropShadow();
    }
}

void Menu_DrawArmorItemRow(int index) {
    Menu_DrawArmorItemDetail(index);
}

void Menu_DrawBattleCommandItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x1F);
}

void Menu_DrawSkillItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x5D);
}
