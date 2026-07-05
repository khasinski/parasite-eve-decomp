/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_FindByFieldValue(int value) {
    MenuWidgetNode *node;
    MenuWidgetNode *result;
    int i;

    node = g_MenuWidgetActiveListHead;
    result = 0;
    while (node != 0) {
        i = 0;
        while (i < 4) {
            if (((int *)&node->field_08)[i] == value) {
                break;
            }
            i++;
        }
        if (i < 4) {
            result = node;
            break;
        }
        node = node->next;
    }
    return result;
}

int MenuWidget_GetChild(int *arg0, int arg1) {
    return arg0[arg1 + 2];
}

MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base) {
    MenuWidgetNode *node;

    node = g_MenuWidgetActiveListHead;
    while (node != 0) {
        if (node->mode == mode && node->selected_base == selected_base) {
            break;
        }
        node = node->next;
    }
    return node;
}
