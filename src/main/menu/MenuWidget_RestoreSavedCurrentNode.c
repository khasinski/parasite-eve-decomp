/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

MenuWidgetNode *g_MenuWidgetActiveListHead;
MenuWidgetNode *g_MenuWidgetCurrentNode;
MenuWidgetNode *g_MenuWidgetSavedNode;

void MenuWidget_RestoreSavedCurrentNode(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *pending;

    node = g_MenuWidgetActiveListHead;
    if (node != 0) {
        pending = g_MenuWidgetSavedNode;
        while (node != 0) {
            if (node == pending) {
                break;
            }
            node = node->next;
        }
        if (node != 0) {
            g_MenuWidgetCurrentNode = node;
        }
    }
    g_MenuWidgetSavedNode = 0;
}
