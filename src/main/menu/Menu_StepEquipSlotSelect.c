#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
void Menu_StepEquipSlotSelect(void)
{
    MenuWidgetNode *node, *current;
    ItemDataRecord *item, *other;
    int selection;
    node = MenuWidget_FindByModeAndSelectedBase(2, 13);
    if (node) {
        if (node->cursor_x >= 0)
            Inv_RememberSelection(0, Inv_GetPackedListItem(MenuWidget_GridCellIndex(node)));
    } else if (g_MenuItemUseMode) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 16);
        if (node && node->cursor_x >= 0)
            Inv_RememberSelection(0, Inv_GetPackedListItem(MenuWidget_GridCellIndex(node)));
    } else if (!D_8009CF1C) {
        Inv_RememberSelection(0, D_8009CF18 ? D_800C0E20.tracked[0] : D_800C0E20.tracked[2]);
    }
    if (D_8009CF1C)
        selection = Inv_RestoreSelection(0);
    else
        selection = D_8009CF18 ? D_800C0E20.tracked[0] : D_800C0E20.tracked[2];
    item = Inv_LookupActiveListData(selection);
    current = MenuWidget_FindByModeAndSelectedBase(2, 7);
    if (current == MenuWidget_GetCurrentNode()) {
        other = Inv_LookupActiveListData(Inv_RestoreSelection(1));
        Menu_DrawEquipStatsDelta(D_8009CF1C ? other : item);
        Menu_StepItemGridScroll(item, other);
    } else {
        Menu_DrawEquipStatsDelta(item);
        if (item) {
            Draw_OffsetCursor(42, -12);
            Draw_PrintNumberWidth4Unk(item->baseStats[2]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
            Draw_OffsetCursor(-45, -14);
            Draw_PrintNumberWidth4Unk(item->baseStats[1]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
            Draw_OffsetCursor(-45, -14);
            Draw_PrintNumberWidth4Unk(item->baseStats[0]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
            Draw_OffsetCursor(-45, -10);
            Draw_AllocSprite(0x87);
            Draw_OffsetCursor(25, 0);
            Draw_AllocSprite(0x88);
        }
    }
}
