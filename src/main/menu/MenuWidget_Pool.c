/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "common.h"
#include "pe1/menu_widget.h"
#include "../../../tools/m2c/m2c_macros.h"

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
#define NULL ((void *)0)

#include "pe1/bounds_check.h"


MenuWidgetNode *MenuWidget_AllocNode(MenuWidgetNode *arg0, void *arg1);

MenuWidgetNode *MenuWidget_AllocNode(MenuWidgetNode *arg0, void *arg1) {
    s32 var_a0;
    s32 var_a0_2;
    s32 var_v0;
    MenuWidgetNode *temp_s0;
    MenuWidgetNode *temp_v1;
    MenuWidgetNode *temp_next;
    MenuWidgetNode **var_a1;
    register void *var_v1 asm("$3");
    MenuWidgetNode *temp_s2;
    MenuWidgetNode *temp_s1;

    temp_s2 = arg0;
    temp_s1 = arg1;
    temp_s0 = g_MenuWidgetFreeListHead;
    if (temp_s0 == NULL) {
        BoundsCheck_AssertStub(0xA);
    }
    var_a0 = 3;
    temp_next = temp_s0->next;
    temp_v1 = g_MenuWidgetActiveListHead;
    var_a1 = &temp_s0->children[1];
    g_MenuWidgetActiveListHead = temp_s0;
    temp_s0->parent = temp_s2;
    temp_s0->update = 0;
    temp_s0->draw = 0;
    g_MenuWidgetFreeListHead = temp_next;
    temp_s0->next = temp_v1;
    do {
        var_a1[2] = 0;
        var_a0 -= 1;
        var_a1--;
    } while (var_a0 >= 0);
    temp_s0->y = 0;
    temp_s0->x = 0;
    temp_s0->selected_base = 0;
    temp_s0->mode = 0;
    temp_s0->flags = 0;
    if (temp_s1 != NULL) {
        var_a0_2 = 0;
        var_v1 = temp_s1;
        while (var_a0_2 < 4 && M2C_FIELD(var_v1, s32 *, 8) != 0) {
            var_a0_2 += 1;
            var_v1 += 4;
        }
        var_v0 = var_a0_2 < 4;
        if (var_v0 != 0) {
            temp_s1->children[var_a0_2] = temp_s0;
        } else {
            BoundsCheck_AssertStub(0xB, var_a1);
        }
    }
    return temp_s0;
}


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
