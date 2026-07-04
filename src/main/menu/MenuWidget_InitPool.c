/* MASPSX_FLAGS: -G8 */
/* CC1_FLAGS: -G8 */

#include "pe1/menu_widget.h"

void MenuWidget_InitPool(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *end;
    MenuWidgetNode *next;

    node = g_MenuWidgetNodePool;
    end = node + 24;

    while (node < end) {
        next = node + 1;
        node->next = next;
        node = next;
    }

    g_MenuWidgetNodePoolSentinel.next = 0;
    g_MenuWidgetFreeListHead = g_MenuWidgetNodePool;
    g_MenuWidgetCurrentNode = 0;
    g_MenuWidgetActiveListHead = 0;
}
