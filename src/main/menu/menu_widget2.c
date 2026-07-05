/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_widget.h"

MenuWidgetNode *g_MenuWidgetCurrentNode;

MenuWidgetNode *g_MenuWidgetCurrentNode;
MenuWidgetNode *g_MenuWidgetSavedNode;

MenuWidgetNode *g_MenuWidgetActiveListHead;
MenuWidgetNode *g_MenuWidgetCurrentNode;
MenuWidgetNode *g_MenuWidgetSavedNode;

void MenuWidget_SetCurrentNode(MenuWidgetNode *node) {
    g_MenuWidgetCurrentNode = node;
}

MenuWidgetNode *MenuWidget_GetCurrentNode(void) {
    return g_MenuWidgetCurrentNode;
}

void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0) {
    g_MenuWidgetSavedNode = g_MenuWidgetCurrentNode;
    g_MenuWidgetCurrentNode = arg0;
}

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
