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

s32 Entity_QueryField(void *arg0) {
    s32 temp_v1;
    s32 temp_v1_2;
    void *var_a1;
    void *tv;
    s32 ta;

    temp_v1 = *M2C_FIELD(arg0, s32 **, 4);
    if (temp_v1 == 0) {
        tv = g_PlayerEntity;
        if (tv == NULL) {
            goto block_12;
        }
        var_a1 = tv;
        goto block_14;
    }
    var_a1 = g_CurrentEntity;
    if ((temp_v1 != M2C_FIELD(var_a1, u8 *, 0xC)) || (*M2C_FIELD(arg0, s32 **, 8) != M2C_FIELD(var_a1, u8 *, 0xD))) {
        var_a1 = g_FieldActorListHead;
        if (var_a1 != NULL) {
            ta = *(volatile s32 *)*(s32 * volatile *)((u8 *)arg0 + 4);
loop_7:
            if ((M2C_FIELD(var_a1, u8 *, 0xC) != ta) || (M2C_FIELD(var_a1, u8 *, 0xD) != *M2C_FIELD(arg0, s32 **, 8)) || (M2C_FIELD(var_a1, s32 *, 0x98) & 0x10)) {
                var_a1 = M2C_FIELD(var_a1, void **, 4);
                if (var_a1 != NULL) {
                    goto loop_7;
                }
            }
            if (var_a1 == NULL) {
block_12:
                goto block_13;
            }
            goto block_14;
        }
block_13:
        *M2C_FIELD(arg0, s32 **, 0xC) = -1;
        return 1;
    }
block_14:
    temp_v1_2 = *M2C_FIELD(arg0, s32 **, 0);
    if (temp_v1_2 == 1) {
        goto case1;
    }
    if (temp_v1_2 < 2) {
        if (temp_v1_2 == 0) {
            goto case0;
        }
        return 1;
    }
    if (temp_v1_2 == 2) {
        goto case2;
    }
    if (temp_v1_2 == 3) {
        goto case3;
    }
    return 1;
case0:
    *M2C_FIELD(arg0, s32 **, 0xC) = (s32) M2C_FIELD(var_a1, u8 *, 0xE);
    goto ret1;
case1:
    *M2C_FIELD(arg0, s32 **, 0xC) = M2C_FIELD(var_a1, s32 *, 0x98);
    goto ret1;
case2:
    *M2C_FIELD(arg0, s32 **, 0xC) = (s32) M2C_FIELD(var_a1, s16 *, 0x16);
    goto ret1;
case3:
    *M2C_FIELD(arg0, s32 **, 0xC) = (s32) M2C_FIELD(var_a1, u8 *, 0xF);
ret1:
    return 1;
}
