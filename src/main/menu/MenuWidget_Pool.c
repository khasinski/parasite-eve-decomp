/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

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
    void *var_a1;
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
    var_a1 = (char *)temp_s0 + 0xC;
    g_MenuWidgetActiveListHead = temp_s0;
    temp_s0->parent = temp_s2;
    temp_s0->update = 0;
    temp_s0->draw = 0;
    g_MenuWidgetFreeListHead = temp_next;
    temp_s0->next = temp_v1;
    do {
        M2C_FIELD(var_a1, s32 *, 8) = 0;
        var_a0 -= 1;
        var_a1 -= 4;
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

