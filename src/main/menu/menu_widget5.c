/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

void MenuWidget_ClearCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 0;
    }
}

void MenuWidget_SetCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 1;
    }
}

int MenuWidget_IsCursorYClear(MenuWidgetNode *ptr) {
    if (ptr) {
        return ptr->cursor_y == 0;
    }

    return 0;
}

MenuWidgetNode *MenuWidget_FindLastMode1WithCursorX(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *result;

    node = g_MenuWidgetActiveListHead;
    result = 0;
    while (node != 0) {
        if (node->mode == 1 && node->cursor_x != 0) {
            result = node;
        }
        node = node->next;
    }
    return result;
}
