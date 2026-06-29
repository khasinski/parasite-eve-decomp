/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK BoundsCheck_AssertStub();

extern void *g_MenuWidgetActiveListHead;
extern void *g_MenuWidgetFreeListHead;

asm(".globl MenuWidget_AllocNode");
asm("MenuWidget_AllocNode = func_800625B8");

void *MenuWidget_AllocNode(s32 arg0, void *arg1) __asm__("func_800625B8");

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
    temp_next = M2C_FIELD(temp_s0, void **, 0);
    temp_v1 = g_MenuWidgetActiveListHead;
    var_a1 = temp_s0 + 0xC;
    g_MenuWidgetActiveListHead = temp_s0;
    M2C_FIELD(temp_s0, s32 *, 4) = temp_s2;
    M2C_FIELD(temp_s0, s32 *, 0x2C) = 0;
    M2C_FIELD(temp_s0, s32 *, 0x30) = 0;
    g_MenuWidgetFreeListHead = temp_next;
    M2C_FIELD(temp_s0, void **, 0) = temp_v1;
    do {
        M2C_FIELD(var_a1, s32 *, 8) = 0;
        var_a0 -= 1;
        var_a1 -= 4;
    } while (var_a0 >= 0);
    M2C_FIELD(temp_s0, s32 *, 0x1C) = 0;
    M2C_FIELD(temp_s0, s32 *, 0x18) = 0;
    M2C_FIELD(temp_s0, s32 *, 0x24) = 0;
    M2C_FIELD(temp_s0, s32 *, 0x20) = 0;
    M2C_FIELD(temp_s0, s32 *, 0x28) = 0;
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
