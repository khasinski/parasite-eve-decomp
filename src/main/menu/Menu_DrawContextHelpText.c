/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_context_help.h"

/* Draws the play-time clock and the help line for the focused menu widget:
 * the item description, ability or option hint selected by its grid cell. */
void Menu_DrawContextHelpText(MenuWidgetNode *panel)
{
    MenuWidgetNode *current;
    ItemDataRecord *data;
    u8 *text;
    int index;
    int mask;
    int value;
    int hint;
    int i;

    text = 0;
    current = MenuWidget_GetCurrentNode();
    Draw_StatePush();
    Draw_OffsetCursor(panel->grid_width - 0x27, 1);
    Draw_PrintTimeValue(GameTime_GetCounterSeconds(2), 1);
    Draw_OffsetCursor(-0x21, 0);
    Draw_AllocSprite(0x8C);
    Draw_StatePop();
    if (current != 0) {
        index = MenuWidget_GridCellIndex(
            MenuWidget_FindByModeAndSelectedBase(2, current->selected_base));
        switch (current->selected_base) {
        case 0:
            if (Menu_GetBattleEquipMode())
                mask = D_8009CEF0 & 0x1F;
            else
                mask = D_8009CEF0 & 0x1EF;
            i = -1;
            do {
                int bit;

                if (index < 0)
                    break;
                bit = mask & 1;
                index -= bit;
                i++;
                mask >>= 1;
            } while (i < 9);
            text = Str_LookupTable10(i + 0x28);
            break;
        case 5:
            if (index < 0)
                break;
            data = Inv_LookupActiveListData(D_8009CF18 ? D_800C0E20.tracked[0]
                                                       : D_800C0E20.tracked[2]);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 1:
            if (D_8009CF8C >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(D_8009CF8C) < 0x13
                    || Inv_GetActiveListItemType(D_8009CF8C) >= 0x16)
                    hint = 1;
                text = Str_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 51:
            if (D_8009CF8C >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(D_8009CF8C) < 0x13
                    || Inv_GetActiveListItemType(D_8009CF8C) >= 0x16)
                    hint = 1;
                text = Str_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupActiveListData(Inv_GetWayneListItemByIndex(index));
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 52:
            if (D_8009CF8C >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(D_8009CF8C) < 0x13
                    || Inv_GetActiveListItemType(D_8009CF8C) >= 0x16)
                    hint = 1;
                text = Str_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupItemData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 7:
            if (D_8009CF1C)
                index = Inv_RestoreSelection(1);
            else
                index = Inv_GetPackedListItem(index);
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 8:
            if (index < Inv_GetPackedListCount())
                text = Str_LookupTableC(Inv_GetPackedListItem(index) + 0xEB);
            break;
        case 12:
            text = Str_LookupTable10(index + 0x4F);
            break;
        case 13:
        case 16:
            if (D_8009CEFC) {
                text = Str_LookupTableC(Menu_GetBattleCountEntry(index) - 1);
                break;
            }
            data = Inv_LookupActiveListData(Inv_GetPackedListItem(index));
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 14:
            Inv_SelectActiveList(0);
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 6:
            value = D_8009CF20->tailData[index] & 0x1F;
            if (value != 0)
                text = Str_LookupTable10(value + (1 - D_8009CF18) * 20 - 1);
            break;
        case 11:
            data = Inv_LookupActiveListData(Inv_RestoreSelection(1));
            value = data->tailData[index] & 0x1F;
            if (value != 0)
                text = Str_LookupTable10(value + (1 - D_8009CF18) * 20 - 1);
            break;
        case 17:
            text = Str_LookupTable4(0x27);
            break;
        case 27:
        case 28:
            text = Str_LookupTable10(0x52);
            break;
        case 29:
            {
                int base = D_8009CF18 * 3 - 0x56;

                text = Str_LookupTable10(index - base);
            }
            break;
        case 32:
            if (index < 4)
                text = Str_LookupTable10(index + 0x31);
            else
                text = 0;
            break;
        case 33:
            text = Str_LookupTable10(index + 0x3A);
            break;
        case 35:
            text = Str_LookupTable10(index + 0x3D);
            break;
        case 36:
            if (MemCard_PollTransferDelay()) {
                Draw_OffsetCursor(6, 5);
                Draw_PrintTextById(0x4C);
                Draw_OffsetCursor(0, 0x10);
                Draw_PrintTextById(0x4D);
                break;
            }
            hint = 0;
            if (MemCard_IsPortPresent(0) || MemCard_IsPortPresent(1))
                hint = 1;
            text = Str_LookupTable4(hint | 0x4E);
            break;
        case 37:
        case 38:
            if (D_8009CF50) {
                if (D_8009CFF8) {
                    Draw_SetTextDimmed(0);
                    Draw_OffsetCursor(6, 5);
                    Draw_PrintTextById(0x58);
                    Draw_OffsetCursor(0, 0x10);
                    Draw_PrintTextById(0x59);
                    break;
                }
                text = Str_LookupTable4(0x5C);
                break;
            }
            text = Str_LookupTable4(Save_GetTitleStyleFlag() ? 0x57 : 0x5B);
            break;
        case 39:
            Draw_SetTextDimmed(0);
            Draw_OffsetCursor(6, 5);
            Draw_PrintTextById(0x50);
            Draw_OffsetCursor(0, 0x10);
            Draw_PrintTextById(0x51);
            break;
        case 46:
            text = Str_LookupTable10(index + 0x3F);
            break;
        case 48:
            text = Str_LookupTable10(index + 0x38);
            break;
        case 49:
            text = Str_LookupTable10(index + 0x42);
            break;
        case 50:
            /* Retail keeps the table base in a register for the row-4
             * override and recomputes the row address on both paths. */
            {
                u8 *hints = D_8009234C;
                u8 *hint;

                if (D_8009CF0C == 1) {
                    if (index == 2)
                        hint = &hints[4];
                    else
                        hint = &D_8009234C[index];
                } else {
                    hint = &D_8009234C[index];
                }
                text = Str_LookupTable10(*hint);
            }
            break;
        case 56:
            Draw_SetTextDimmed(0);
            text = Str_LookupTable10(0x44);
            break;
        case 58:
            text = Str_LookupTable10(index + 0x35);
            break;
        case 59:
            if (D_8009CF0C == 2)
                text = Str_LookupTable10(index + 0x4D);
            else
                text = Str_LookupTable10(index + 0x4B);
            break;
        case 60:
            {
                int base = D_8009CF18 * 3 - 0x48;

                text = Str_LookupTable10(index - base);
            }
            break;
        }
    }
    if (text != 0) {
        Draw_OffsetCursor(6, 5);
        Draw_PrintRawText(text);
        Draw_SetColor(0x808080);
    }
}
