/* MASPSX_FLAGS: -G8 */
/* CC1_FLAGS: -G8 */

#include "pe1/menu_widget.h"

void MenuWidget_DestroyNodeRecursive(MenuWidgetNode *node);

void MenuWidget_NavScrollTo(int selected_base) {
    MenuWidgetNode *node;
    int one;

    node = g_MenuWidgetActiveListHead;
    one = 1;
    while (node != 0) {
        if (node->mode == one && node->selected_base == selected_base) {
            break;
        }
        node = node->next;
    }

    MenuWidget_DestroyNodeRecursive(node);
}
