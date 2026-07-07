/* CC1_FLAGS: -g3 -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
extern struct { char _[16]; } D_8009D20C_o __asm__("g_FieldActorListHead");
#define g_FieldActorListHead (*(void **)&D_8009D20C_o)
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(void **)&g_PlayerEntity_o)
extern struct { char _[16]; } D_8009D2F0_o __asm__("g_CurrentEntity");
#define g_CurrentEntity (*(void **)&D_8009D2F0_o)

s32 Task_SetEntityParentLink(void *arg0) {
    register s32 temp_v0 asm("v0");
    register void *var_a2 asm("a2");

    temp_v0 = *M2C_FIELD(arg0, s32 **, 0);
    if (temp_v0 == 0) {
        temp_v0 = (s32) g_PlayerEntity;
        if (temp_v0 != 0) {
            var_a2 = (void *) temp_v0;
            goto block_10;
        }
        goto block_9;
    }
    var_a2 = g_FieldActorListHead;
    if (var_a2 != NULL) {
        register s32 temp_a1 asm("a1");
        temp_a1 = temp_v0;
loop_4:
        if ((M2C_FIELD(var_a2, u8 *, 0xC) != temp_a1) || (M2C_FIELD(var_a2, u8 *, 0xD) != *M2C_FIELD(arg0, s32 **, 4)) || (M2C_FIELD(var_a2, s32 *, 0x98) & 0x10)) {
            var_a2 = M2C_FIELD(var_a2, void **, 4);
            if (var_a2 != NULL) {
                goto loop_4;
            }
        }
        if (var_a2 != NULL) {
            goto block_10;
        }
    }
block_9:
    return 1;
block_10:
    M2C_FIELD(g_CurrentEntity, void **, 0x18C) = var_a2;
    M2C_FIELD(var_a2, s32 *, 0x98) = (s32) (M2C_FIELD(var_a2, s32 *, 0x98) | 0x100000);
    M2C_FIELD(g_CurrentEntity, s32 *, 0x98) = (s32) (M2C_FIELD(g_CurrentEntity, s32 *, 0x98) | 0x600000);
    return 1;
}
