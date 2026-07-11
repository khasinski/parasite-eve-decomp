/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

void func_80064A54(MenuWidgetNode *node);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);

void MenuWidget_DestroyNodeRecursive(MenuWidgetNode *target) {
    MenuWidgetNode *node;
    MenuWidgetNode *prev;
    MenuWidgetNode *next;
    int i;

    node = g_MenuWidgetActiveListHead;
    prev = 0;
    if (node != 0) {
        while (node != target) {
            prev = node;
            node = node->next;
            if (node == 0) {
                break;
            }
        }

        if (node != 0) {
            if (prev != 0) {
                prev->next = node->next;
            } else {
                g_MenuWidgetActiveListHead = node->next;
            }

            node->next = g_MenuWidgetFreeListHead;
            g_MenuWidgetFreeListHead = node;

            if (node->mode == 2) {
                func_80064A54(node);
            }

            for (i = 0; i < 4; i++) {
                if (((MenuWidgetNode **)&node->field_08)[i] != 0) {
                    MenuWidget_DestroyNodeRecursive(((MenuWidgetNode **)&node->field_08)[i]);
                }
            }

            if (MenuWidget_GetCurrentNode() == node) {
                MenuWidget_SetCurrentNode(node->field_04);
            }

            node = g_MenuWidgetActiveListHead;
            if (node != 0) {
                do {
                    if (node->field_04 == target) {
                        node->field_04 = 0;
                    }

                    if (node->mode == 2) {
                        if (node->field_78 == target) {
                            node->field_78 = 0;
                        }
                        if (node->field_7C == target) {
                            node->field_7C = 0;
                        }
                    }

                    next = node;
                    for (i = 0; i < 4; i++) {
                        if (((MenuWidgetNode **)&next->field_08)[0] == target) {
                            ((MenuWidgetNode **)&next->field_08)[0] = 0;
                        }
                        next = (MenuWidgetNode *)((char *)next + 4);
                    }

                    node = node->next;
                } while (node != 0);
            }
        }
    }
}
