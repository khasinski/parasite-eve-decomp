/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_GridCellIndex();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
M2C_UNK Menu_PlayMoveSound();
s32 Draw_GetBlendColor();
M2C_UNK Draw_BlendColor();
void *MenuWidget_GetChild();
M2C_UNK MenuWidget_DestroyNode();
extern s32 D_8009CFE0;
extern struct { char _[16]; } g_SavedDrawBlendColor_o __asm__("g_SavedDrawBlendColor");
#define g_SavedDrawBlendColor (*(s32 *)&g_SavedDrawBlendColor_o)

s32 Menu_StepColorSelect(s32 arg0, s32 arg1) {
    s32 temp_s0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_a0;
    s32 var_v0;
    s32 clamp;
    s32 var_v0_2;
    void *temp_v0;

    temp_s0 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0)) * 8;
    temp_v1 = Draw_GetBlendColor();
    if (!(arg1 & 0x1000)) {
        goto check4000;
    }
    var_a0 = temp_v1 & ~(0xFF << temp_s0);
    temp_v1_2 = ((temp_v1 >> temp_s0) & 0xFF) + 2;
    if (temp_v1_2 < 0xE9) {
        var_v0 = temp_v1_2 << temp_s0;
        goto blend;
    }
    clamp = 0xE8;
    goto clampshift;
check4000:
    if (!(arg1 & 0x4000)) {
        goto check10000;
    }
    var_a0 = temp_v1 & ~(0xFF << temp_s0);
    temp_v1_3 = ((temp_v1 >> temp_s0) & 0xFF) - 2;
    if (temp_v1_3 >= 0x20) {
        var_v0 = temp_v1_3 << temp_s0;
        goto blend;
    }
    clamp = 0x20;
clampshift:
    var_v0 = clamp << temp_s0;
blend:
    Draw_BlendColor(var_a0 | var_v0);
    Menu_PlayMoveSound();
    return 1;
check10000:
    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetChild(arg0, 1);
        temp_v0_2 = MenuWidget_GridCellIndex(temp_v0);
        if (temp_v0_2 == 0) {
            goto take;
        }
        if (temp_v0_2 != 1) {
            goto setcur;
        }
        Draw_BlendColor(0x404040);
take:
        g_SavedDrawBlendColor = Draw_GetBlendColor();
        MenuWidget_DestroyNode(arg0);
        goto confirm;
setcur:
        M2C_FIELD(temp_v0, s32 *, 0x44) = 0;
        M2C_FIELD(temp_v0, s32 *, 0x48) = 0;
        MenuWidget_SetCurrentNode(temp_v0);
        M2C_FIELD(MenuWidget_GetChild(arg0, 0), s32 *, 0x44) = -1;
confirm:
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Draw_BlendColor(D_8009CFE0);
        Menu_PlayCancelSound();
    }
    return 1;
}
