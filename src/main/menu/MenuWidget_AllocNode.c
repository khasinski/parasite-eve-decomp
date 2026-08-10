#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK BoundsCheck_AssertStub();

#define NODE_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(MenuWidgetNode, member)))

void *MenuWidget_AllocNode(s32 arg0, void *arg1);

void *MenuWidget_AllocNode(s32 arg0, void *arg1) {
    s32 var_a0;
    s32 var_a0_2;
    s32 var_v0;
    void *temp_s0;
    void *temp_v1;
    void *temp_next;
    void *var_a1;
    register void *var_v1 asm("$3");
    s32 temp_s2;
    void *temp_s1;

    temp_s2 = arg0;
    temp_s1 = arg1;
    temp_s0 = g_MenuWidgetFreeListHead;
    if (temp_s0 == NULL) {
        BoundsCheck_AssertStub(0xA);
    }
    var_a0 = 3;
    temp_next = NODE_FIELD(temp_s0, void **, next);
    temp_v1 = g_MenuWidgetActiveListHead;
    var_a1 = temp_s0 + 0xC;
    g_MenuWidgetActiveListHead = temp_s0;
    NODE_FIELD(temp_s0, s32 *, parent) = temp_s2;
    NODE_FIELD(temp_s0, s32 *, update) = 0;
    NODE_FIELD(temp_s0, s32 *, field_30) = 0;
    g_MenuWidgetFreeListHead = temp_next;
    NODE_FIELD(temp_s0, void **, next) = temp_v1;
    do {
        M2C_FIELD(var_a1, s32 *, 8) = 0;
        var_a0 -= 1;
        var_a1 -= 4;
    } while (var_a0 >= 0);
    NODE_FIELD(temp_s0, s32 *, y) = 0;
    NODE_FIELD(temp_s0, s32 *, x) = 0;
    NODE_FIELD(temp_s0, s32 *, selected_base) = 0;
    NODE_FIELD(temp_s0, s32 *, mode) = 0;
    NODE_FIELD(temp_s0, s32 *, field_28) = 0;
    if (temp_s1 != NULL) {
        var_a0_2 = 0;
        var_v1 = temp_s1;
loop_6:
        {
            s32 temp_v0;
            temp_v0 = M2C_FIELD(var_v1, s32 *, 8);
            if (temp_v0 == 0) {
                temp_v0 = var_a0_2 < 4;
                var_v0 = temp_v0;
                goto block_8;
            }
        }
        var_a0_2 += 1;
        var_v1 += 4;
        if (var_a0_2 >= 4) {
            asm volatile("" : "=r"(var_a0_2) : "0"(var_a0_2));
            var_v0 = var_a0_2 < 4;
        } else {
            goto loop_6;
        }
block_8:
        if (var_v0 != 0) {
            void *temp_v0;
            temp_v0 = (void *) (var_a0_2 * 4);
            temp_v0 = (void *) ((s32) temp_v0 + (s32) temp_s1);
            M2C_FIELD(temp_v0, void **, 8) = temp_s0;
        } else {
            BoundsCheck_AssertStub(0xB, var_a1);
        }
    }
    return temp_s0;
}

#undef NODE_FIELD
