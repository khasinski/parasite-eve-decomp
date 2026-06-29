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

int Gte_Atan2(int arg0, int arg1);

extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(void **)&g_PlayerEntity_o)
extern struct { char _[16]; } D_8009D20C_o __asm__("g_FieldActorListHead");
#define g_FieldActorListHead (*(void **)&D_8009D20C_o)
extern struct { char _[16]; } D_8009D2F0_o0 __asm__("g_CurrentEntity");
extern struct { char _[16]; } D_8009D2F0_o1 __asm__("g_CurrentEntity");
#define D_8009D2F0_0 (*(void **)&D_8009D2F0_o0)
#define D_8009D2F0_1 (*(void **)&D_8009D2F0_o1)

s32 Task_GetAngleToEntity(void *arg0) {
    register s32 temp_v0 asm("$2");
    s32 temp_a0;
    register void *var_a2 asm("$6");
    s32 angle;
    s32 dx;
    s32 dz;

    temp_v0 = *M2C_FIELD(arg0, s32 **, 0);
    if (temp_v0 == 0) {
        void *temp_ptr;

        temp_ptr = g_PlayerEntity;
        var_a2 = temp_ptr;
        if (temp_ptr == NULL) {
            goto fail;
        }
        goto found;
    } else {
        temp_a0 = temp_v0;
        var_a2 = g_FieldActorListHead;
        if (var_a2 == NULL) {
            goto fail;
        }
loop:
        if ((M2C_FIELD(var_a2, u8 *, 0xC) != temp_a0) ||
            (M2C_FIELD(var_a2, u8 *, 0xD) != *M2C_FIELD(arg0, s32 **, 4)) ||
            (M2C_FIELD(var_a2, s32 *, 0x98) & 0x10)) {
            var_a2 = M2C_FIELD(var_a2, void **, 4);
            if (var_a2 != NULL) {
                goto loop;
            }
        }
        if (var_a2 != NULL) {
            goto found;
        }
    }

fail:
    *M2C_FIELD(arg0, s32 **, 8) = -1;
    return 1;

found:
    __asm__ volatile(
        "lui   $2, %%hi(g_CurrentEntity)\n"
        "lw    $2, %%lo(g_CurrentEntity)($2)\n"
        "lw    $3, 0x28(%2)\n"
        "lw    %1, 0x28($2)\n"
        "lw    %0, 0x30($2)\n"
        "lw    $2, 0x30(%2)\n"
        "subu  %1, %1, $3\n"
        "subu  %0, %0, $2\n"
        "sra   %0, %0, 16"
        : "=&r"(dz), "=&r"(dx)
        : "r"(var_a2)
        : "$2", "$3");
    angle = 0x1400 - Gte_Atan2(dz, dx >> 16);
    if (angle >= 0x1001) {
        angle -= 0x1000;
    }
    angle -= M2C_FIELD(D_8009D2F0_1, s16 *, 0x3A);
    if (angle < 0) {
        angle += 0x1000;
    }
    *M2C_FIELD(arg0, s32 **, 8) = angle;
    return 1;
}
