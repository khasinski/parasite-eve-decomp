/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_inventory.h"
MenuWidgetNode *g_MenuWidgetActiveListHead;
MenuWidgetNode *g_MenuWidgetFreeListHead;
void MenuWidget_DestroyNodeRecursive(MenuWidgetNode *node) {
    MenuWidgetNode *current = g_MenuWidgetActiveListHead;
    MenuWidgetNode *previous = 0;
    int i;
    if (current) {
        while (current) {
            if (current == node) {
                break;
            }
            previous = current;
            current = current->next;
        }
        if (current) {
            if (previous) {
                previous->next = current->next;
            } else {
                g_MenuWidgetActiveListHead = current->next;
            }
            current->next = g_MenuWidgetFreeListHead;
            g_MenuWidgetFreeListHead = current;
            if (current->mode == 2) {
                func_80064A54(current);
            }
            for (i = 0; i < 4; i++) {
                if (current->children[i]) {
                    MenuWidget_DestroyNodeRecursive(current->children[i]);
                }
            }
            if (MenuWidget_GetCurrentNode() == current) {
                MenuWidget_SetCurrentNode(current->parent);
            }
            for (current = g_MenuWidgetActiveListHead; current; current = current->next) {
                if (current->parent == node) current->parent = 0;
                if (current->mode == 2) {
                    if (current->linkedPrevious == node) current->linkedPrevious = 0;
                    if (current->linkedNext == node) current->linkedNext = 0;
                }
                for (i = 0; i < 4; i++) {
                    if (current->children[i] == node) current->children[i] = 0;
                }
            }
        }
    }
}
