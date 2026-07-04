/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

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
