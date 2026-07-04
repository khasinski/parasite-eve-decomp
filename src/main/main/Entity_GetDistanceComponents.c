/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
extern void * g_FieldActorListHead[];
#define g_FieldActorListHead (g_FieldActorListHead[0])
extern void * g_PlayerEntity[];
#define g_PlayerEntity (g_PlayerEntity[0])
extern void * g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])

#define LOAD_D2F0(dst)                                                \
    asm volatile(                                                     \
        "lui\t%0, %%hi(g_CurrentEntity)\n"                                 \
        "lw\t%0, %%lo(g_CurrentEntity)(%0)"                                \
        : "=r"(dst))

#define LOAD_D2F0_AND_OUT(dst, out, base)                             \
    asm volatile(                                                     \
        "lui\t%0, %%hi(g_CurrentEntity)\n"                                 \
        "lw\t%0, %%lo(g_CurrentEntity)(%0)\n"                              \
        "lw\t%1, 0x8(%2)"                                             \
        : "=r"(dst), "=r"(out)                                        \
        : "r"(base))

s32 Entity_GetDistanceComponents(void *arg0) {
    s32 *temp_a0;
    s32 *temp_t0;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_a3;
    s32 temp_a3_2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    register s32 key0 asm("$2");
    void *node;

    key0 = *M2C_FIELD(arg0, s32 **, 0);
    if (key0 == 0) {
        void *tmp;
        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            goto block_9;
        }
        node = tmp;
        goto block_10;
    }
    node = g_FieldActorListHead;
    if (node != NULL) {
        s32 key;
        key = key0;
loop_4:
        if ((M2C_FIELD(node, u8 *, 0xC) != key) || (M2C_FIELD(node, u8 *, 0xD) != *M2C_FIELD(arg0, s32 **, 4)) || (M2C_FIELD(node, s32 *, 0x98) & 0x10)) {
            node = M2C_FIELD(node, void **, 4);
            if (node != NULL) {
                goto loop_4;
            }
        }
        if (node != NULL) {
            goto block_10;
        }
    }
    goto block_9;

block_9:
    *M2C_FIELD(arg0, s32 **, 8) = -1;
    return 1;

block_10:
    {
        void *state;
        s32 node_x;
        register s32 state_x asm("$7");
        s32 *out;
        LOAD_D2F0(state);
        node_x = M2C_FIELD(node, s32 *, 0x28);
        state_x = M2C_FIELD(state, s32 *, 0x28);
        out = M2C_FIELD(arg0, s32 **, 8);
        var_v0 = state_x - node_x;
        if (var_v0 < 0) {
            var_v0 = node_x - state_x;
        }
        *out = var_v0;
    }
    {
        void *state;
        s32 *out;
        LOAD_D2F0_AND_OUT(state, out, arg0);
        temp_a3_2 = M2C_FIELD(state, s32 *, 0x2C);
        temp_v0_2 = M2C_FIELD(node, s32 *, 0x2C);
        temp_a2 = *out;
        temp_v1_2 = temp_a3_2 - temp_v0_2;
        if (temp_v1_2 >= 0) {
            var_v0_2 = temp_a2 + temp_v1_2;
        } else {
            var_v0_2 = temp_a2 + (temp_v0_2 - temp_a3_2);
        }
        *out = var_v0_2;
    }
    {
        void *state;
        register s32 *out asm("$4");
        LOAD_D2F0_AND_OUT(state, out, arg0);
        temp_a2_2 = M2C_FIELD(state, s32 *, 0x30);
        temp_v0_3 = M2C_FIELD(node, s32 *, 0x30);
        temp_a1 = *out;
        temp_v1_3 = temp_a2_2 - temp_v0_3;
        if (temp_v1_3 >= 0) {
            var_v0_3 = temp_a1 + temp_v1_3;
        } else {
            var_v0_3 = temp_a1 + (temp_v0_3 - temp_a2_2);
        }
        *out = var_v0_3;
    }
    return 1;
}
