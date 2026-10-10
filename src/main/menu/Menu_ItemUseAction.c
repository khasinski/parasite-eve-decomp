/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/inventory.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_item_record.h"

int g_MenuEquipMode;
int g_MenuItemRenameMode;

int Inv_GetPackedListItem(int arg0);
void Menu_CloseContextHelpPanel(void);
int Inv_GetActiveListItem(int arg0);

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_SetColor(int arg0);
void Draw_PrintTextById(int arg0);
void Draw_PrintNumberWidth3Unk(int value);
int Draw_GetBlendColor(void);

void Menu_ItemUseAction(struct MenuWidgetNode *arg0, int arg1) {
    int saved;
    ItemDataRecord *ptr;

    if (arg1 != 0) {
        saved = Inv_GetPackedListItem(MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 0xD)));
        MenuWidget_NavScrollTo(0xF);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xD);
        MenuWidget_NavScrollTo(0x18);
        MenuWidget_NavScrollTo(0x30);

        if (g_MenuEquipMode != 0) {
            MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
        } else {
            if (MenuWidget_GetCurrentNode() != 0) {
                MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
            }
            MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        }

        g_MenuItemRenameMode = 0;
        Menu_CloseContextHelpPanel();
        ptr = Inv_LookupActiveListData(saved);
        ptr->tailCount++;
        Menu_CreateItemUsePanel(Inv_GetActiveListItem(saved));
    }
}

void Menu_DrawBlendColorChannelRow(int arg0) {
    int shift;
    int value;

    Draw_StatePush();
    shift = arg0 << 3;
    Draw_SetColor(0x80 << shift);
    Draw_PrintTextById(arg0 + 0x35);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(0x0A, 3);
    value = (Draw_GetBlendColor() >> shift) & 0xFF;
    Draw_PrintNumberWidth3Unk((value - 0x20) >> 1);
    Draw_StatePop();
}
