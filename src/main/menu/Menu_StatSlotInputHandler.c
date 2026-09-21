/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_inventory.h"

int g_MenuItemUseMode;

int Menu_StatSlotInputHandler(MenuWidgetNode *node, unsigned int flags) {
    MenuWidgetNode *current = MenuWidget_GetChild(node, 0);

    if (flags & 0x4000) {
        MenuWidgetNode *other;
        int x, offset;

        current->cursor_x = -1;
        current = MenuWidget_FindByModeAndSelectedBase(2, 16);
        current->cursor_x = 0;
        current->cursor_y = 0;
        MenuWidget_ClampScroll(current);
        MenuWidget_SetCurrentNode(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 15);
        other = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 13), 0);
        x = current->x;
        offset = (g_MenuItemUseMode ? 176 : other->popup_node ? 162 : 156) - x;
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 11);
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 47);
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        Menu_OpenBonusPointSpendDialog(current, MenuWidget_GridCellIndex(current) + 5);
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        func_800490B0();
        Menu_PlayCancelSound();
    }
    return 1;
}
