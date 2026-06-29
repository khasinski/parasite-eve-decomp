/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int arg1);
extern int MenuWidget_GridCellIndex(MenuWidgetNode *node);
extern void Inv_TransferItemAlt(void);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern void Inv_ClearDisplaySlots(void);
extern void MenuWidget_InitPool(void);
extern void Inv_SetActiveList(int arg0, int arg1);
extern void Menu_PlayConfirmSound(void);

int Menu_SaveLoadNavigate(MenuWidgetNode *arg0, unsigned int flags) {
    MenuWidgetNode *node;
    int mode;

    if (flags & 0x10000) {
        node = MenuWidget_GetChild(arg0, 0);
        mode = MenuWidget_GridCellIndex(node);

        switch (mode) {
            case 0:
                Inv_TransferItemAlt();
                node->cursor_y = 2;
                break;

            case 1:
                node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
                node->cursor_y = 0;
                node->cursor_x = 0;
                MenuWidget_SetCurrentNode(node);
                break;

            case 2:
                Inv_ClearDisplaySlots();
                MenuWidget_InitPool();
                Inv_SetActiveList(0xA, 0);
                break;

            default:
                return 1;
        }

        Menu_PlayConfirmSound();
    }

    return 1;
}
