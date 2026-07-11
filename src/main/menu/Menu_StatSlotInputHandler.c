/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

extern int D_8009CF30;

extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
extern int MenuWidget_GridCellIndex(MenuWidgetNode *node);
extern void MenuWidget_ClampScroll(MenuWidgetNode *node);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
extern void MenuWidget_ClearCursorY(MenuWidgetNode *node);
extern void Menu_OpenBonusPointSpendDialog(MenuWidgetNode *node, int stat);
extern void Menu_PlayMoveSound(void);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayCancelSound(void);
extern void func_800490B0(void);

int Menu_StatSlotInputHandler(MenuWidgetNode *node, int input) {
    MenuWidgetNode *child;
    MenuWidgetNode *stat_node;
    MenuWidgetNode *detail_node;
    int dx;
    int target_x;

    child = MenuWidget_GetChild(node, 0);

    if ((input & 0x4000) != 0) {
        child->cursor_x = -1;

        stat_node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        stat_node->cursor_x = 0;
        stat_node->cursor_y = 0;
        MenuWidget_ClampScroll(stat_node);
        MenuWidget_SetCurrentNode(stat_node);

        stat_node = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
        detail_node = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);

        if (D_8009CF30 != 0) {
            target_x = 0xB0;
        } else if (detail_node->field_80 != 0) {
            target_x = 0xA2;
        } else {
            target_x = 0x9C;
        }

        dx = target_x - stat_node->value_18;
        MenuWidget_OffsetPosition(stat_node, dx, 0);
        MenuWidget_ClearCursorY(stat_node);

        stat_node = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
        MenuWidget_OffsetPosition(stat_node, dx, 0);
        MenuWidget_ClearCursorY(stat_node);

        stat_node = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
        MenuWidget_OffsetPosition(stat_node, dx, 0);
        MenuWidget_ClearCursorY(stat_node);

        Menu_PlayMoveSound();
        return 1;
    }

    if ((input & 0x10000) != 0) {
        Menu_OpenBonusPointSpendDialog(child, MenuWidget_GridCellIndex(child) + 5);
        Menu_PlayConfirmSound();
        return 1;
    }

    if ((input & 0x40) != 0) {
        func_800490B0();
        Menu_PlayCancelSound();
    }

    return 1;
}
