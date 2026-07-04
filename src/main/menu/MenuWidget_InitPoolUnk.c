/* MASPSX_FLAGS: -G8 */
/* CC1_FLAGS: -G8 */

#include "pe1/menu_widget.h"

void MenuWidget_InitPoolUnk(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *end;
    MenuWidgetNode *next;

    node = (MenuWidgetNode *)g_MenuWidgetNodePool;
    end = node + 24;

    while (node < end) {
        next = node + 1;
        node->next = next;
        node = next;
    }

    g_MenuWidgetNodePoolSentinel.next = 0;
    g_MenuWidgetFreeListHead = (MenuWidgetNode *)&g_MenuWidgetNodePoolSentinel - 23;
    g_MenuWidgetCurrentNode = 0;
    g_MenuWidgetActiveListHead = 0;
}
