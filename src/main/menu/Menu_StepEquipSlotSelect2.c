/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
#define CLAMP_EQUIP_PREVIEW(out, item, stat_index, active_stat) do { \
    int desired_ = (active_stat); \
    int current_ = D_8009CFC8; \
    int raw_ = (item)->bonusStats[(stat_index)]; \
    if ((current_ == desired_ && raw_ + D_8009CFCC < 1000) || \
        (current_ != desired_ && raw_ < 1000)) { \
        (out) = (item)->bonusStats[(stat_index)]; \
        if (D_8009CFC8 == (active_stat)) { \
            (out) += D_8009CFCC; \
        } \
    } else { \
        (out) = 999; \
    } \
} while (0)

void Menu_StepEquipSlotSelect2(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *cursor_node;
    ItemDataRecord *item;
    int slot;
    int value;

    node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    if (node != 0) {
        if (node->cursor_x >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (D_8009CF30 != 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        if (node != 0 && node->cursor_x >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (D_8009CF1C == 0) {
        if (D_8009CF18 != 0) {
            Inv_RememberSelection(0, D_800C0E20.tracked[0]);
        } else {
            Inv_RememberSelection(0, D_800C0E20.tracked[2]);
        }
    }

    slot = Inv_RestoreSelection(0);
    Draw_OffsetCursor(4, 4);
    Sfx_DrawActiveListSlot(slot);
    Draw_OffsetCursor(-4, -4);

    item = Inv_LookupActiveListData(slot);
    D_8009CF18 = (item->kind != 9);
    Menu_DrawEquipStatsDelta(item);

    if ((D_8009CF1C != 0 || D_8009CF30 != 0 || D_8009CF34 != 0 || D_8009CF38 != 0) &&
        item != 0) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(item->baseStats[2]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(item->baseStats[1]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(item->baseStats[0]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }

    if (D_8009CFC4 >= 0) {
        Draw_OffsetCursor(0x28, -0x32);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);
        Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(item->bonusStats[2]);

        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0xA, -0xA);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);

        CLAMP_EQUIP_PREVIEW(value, item, 0, 0);
        Draw_SetStatCompareColor(item->bonusStats[0], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 1, 1);
        Draw_SetStatCompareColor(item->bonusStats[1], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 2, 2);
        Draw_SetStatCompareColor(item->bonusStats[2], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_SetColor(0x808080);
    }

    cursor_node = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    {
        void (*set_cursor_y)(MenuWidgetNode *node);
        set_cursor_y = item->tailCount != 0 ? MenuWidget_ClearCursorY : MenuWidget_SetCursorY;
        set_cursor_y(cursor_node);
    }
}
