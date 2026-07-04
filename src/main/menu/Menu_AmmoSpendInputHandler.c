/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Spend_Ammo();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
M2C_UNK Menu_PlayMoveSound();
M2C_UNK Menu_PlayErrorSound();
M2C_UNK Inv_RebuildSelectableMask();
M2C_UNK Inv_StepScrollDisplay();
s32 func_8005E120();
M2C_UNK MenuWidget_DestroyNode();
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuSpendArrowDirection;

s32 Menu_AmmoSpendInputHandler(s32 arg0, s32 arg1) {
    if (arg1 & 0x1000) {
        if (Spend_Ammo(-func_8005E120()) != 0) {
            Menu_PlayErrorSound();
        } else {
            Menu_PlayMoveSound();
        }
        g_MenuSpendArrowDirection = 1;
    } else if (arg1 & 0x4000) {
        if (Spend_Ammo(func_8005E120()) != 0) {
            Menu_PlayErrorSound();
        } else {
            Menu_PlayMoveSound();
        }
        g_MenuSpendArrowDirection = 0;
    } else {
        if (arg1 & 0x10000) {
            Inv_StepScrollDisplay();
            g_InvSwapTargetIndex = -1;
            g_InvSelectedItemIndex = -1;
            Inv_RebuildSelectableMask();
            MenuWidget_DestroyNode(arg0);
            Menu_PlayConfirmSound();
        }
    }
    if (arg1 & 0x40) {
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Inv_RebuildSelectableMask();
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}
