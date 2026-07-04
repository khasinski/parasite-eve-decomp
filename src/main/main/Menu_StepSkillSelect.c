/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_FindByModeAndSelectedBase();
s32 MenuWidget_GridCellIndex();
M2C_UNK MenuWidget_NavScrollTo();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_StepInventoryRoot();
M2C_UNK Inv_SetActiveList();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
s32 func_80052F0C();
s32 Inv_RestoreSelection();
M2C_UNK Inv_TransferItemBetweenLists();
s32 MenuWidget_GetChild();
M2C_UNK MenuWidget_DestroyNode();
extern s32 g_MenuSelectedItemList;
extern s32 g_MenuSelectedItemSlot;
extern s32 g_MenuSelectedItemIndex;
extern struct { char _[16]; } D_800A1888_o __asm__("g_StatBaseTable");
#define g_StatBaseTable (*(M2C_UNK *)&D_800A1888_o)
extern struct { char _[16]; } D_800C0E20_o __asm__("g_AyaEquippedWeaponSlot");
#define g_AyaEquippedWeaponSlot (*(M2C_UNK *)&D_800C0E20_o)

s32 Menu_StepSkillSelect(s32 arg0, s32 arg1) {
    M2C_UNK var_a0;
    s32 *temp_a1;
    s32 pad_[1];
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    s32 var_v0;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (temp_v0 != 0) {
            if (temp_v0 == 1) {
                goto shared_1;
            }
            return 1;
        }
        {
            MenuWidget_DestroyNode(arg0);
            Inv_TransferItemBetweenLists(g_MenuSelectedItemList, g_MenuSelectedItemSlot, g_MenuSelectedItemIndex, *(s32 *)((u8 *)&D_800A1888_o + (g_MenuSelectedItemIndex * 4)));
            temp_a1 = (s32 *)((u8 *)&D_800A1888_o + (g_MenuSelectedItemIndex * 4));
            temp_v0_2 = *temp_a1;
            *temp_a1 = temp_v0_2 - (temp_v0_2 < 0x3E7);
            MenuWidget_NavScrollTo(0x2C, temp_a1);
            MenuWidget_NavScrollTo(0xB);
            MenuWidget_NavScrollTo(0xA);
            MenuWidget_NavScrollTo(5);
            MenuWidget_NavScrollTo(6);
            MenuWidget_NavScrollTo(7);
            var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0);
            if (var_v0 == 0) {
                var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
            }
            MenuWidget_SetCurrentNode(var_v0);
            var_s1 = 0;
            Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
            do {
                temp_s0 = Inv_RestoreSelection(var_s1);
                if ((func_80052F0C() == 0) && ((var_a0 = 2, (temp_s0 == M2C_FIELD(&g_AyaEquippedWeaponSlot, s8 *, 0))) || (var_a0 = 3, (temp_s0 == M2C_FIELD(&g_AyaEquippedWeaponSlot, s8 *, 2))))) {
                    Inv_SetActiveList(var_a0, 0);
                }
                var_s1 += 1;
            } while (var_s1 < 2);
            Menu_PlayConfirmSound();
            return 1;
        }
    } else {
        if (arg1 & 0x40) {
shared_1:
            MenuWidget_DestroyNode(arg0);
            Menu_PlayCancelSound();
        }
        return 1;
    }
}
