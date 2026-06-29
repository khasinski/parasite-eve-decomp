/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MenuWidget_FindByModeAndSelectedBase();
s32 MenuWidget_GetCurrentNode();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK func_800451D0();
M2C_UNK func_80052E30();
s32 func_80052F0C();
s32 func_80052F70();
M2C_UNK Inv_RebuildSelectableMask();
s32 Inv_TestSelectionBit();
M2C_UNK Inv_InitSlotDisplay();
s32 Inv_BuildCompatibleWeaponBitset();
s32 MenuWidget_GridCellIndex();
extern s32 g_InvAmmoSpendActiveList;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapSourceList;
extern struct { char _[16]; } D_8009CF90_o __asm__("g_InvSwapSourceList");
extern struct { char _[16]; } D_8009CF94_o __asm__("g_InvSwapTargetIndex");
#define g_InvSwapTargetIndex (*(M2C_UNK *)&D_8009CF94_o)

void Menu_OnInventoryItemConfirm(s32 arg0) {
    s32 temp_v0;
    s32 var_s0;
    s32 var_s0_2;
    void *temp_s0;
    void *temp_s0_2;
    void *temp_s1;
    void *temp_s1_2;

    g_InvAmmoSpendActiveList = func_80052F0C();
    g_InvSelectedItemIndex = arg0;
    temp_v0 = Inv_BuildCompatibleWeaponBitset(arg0);
    if (temp_v0 == 0) {
        g_InvSelectedItemIndex = -1;
        Inv_RebuildSelectableMask();
        return;
    }
    if (temp_v0 == 1) {
        temp_s0 = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
        temp_s1 = MenuWidget_FindByModeAndSelectedBase(2, 0xE);
        if ((temp_s0 != NULL) && (MenuWidget_GridCellIndex(temp_s0) != 0) && (temp_s1 != NULL)) {
            M2C_FIELD(temp_s0, s32 *, 0x44) = -1;
            func_80052E30(0);
            var_s0 = 0;
loop_7:
            if ((var_s0 < func_80052F70()) && (Inv_TestSelectionBit(var_s0) == 0)) {
                var_s0 += 1;
                goto loop_7;
            }
            M2C_FIELD(temp_s1, s32 *, 0x44) = 0;
            M2C_FIELD(temp_s1, s32 *, 0x48) = (s32) (var_s0 & -(var_s0 < func_80052F70()));
            MenuWidget_SetCurrentNode(temp_s1);
        }
        {
            void *cf90_addr;
            void *cf94_addr;
            s32 cf88_value;
            __asm__ volatile(
                "lui %0, %%hi(g_InvSwapSourceList)\n"
                "addiu %0, %0, %%lo(g_InvSwapSourceList)"
                : "=r"(cf90_addr));
            cf88_value = g_InvAmmoSpendActiveList;
            __asm__ volatile(
                "lui %0, %%hi(g_InvSwapTargetIndex)\n"
                "addiu %0, %0, %%lo(g_InvSwapTargetIndex)"
                : "=r"(cf94_addr));
            g_InvSwapSourceList = cf88_value;
            Inv_InitSlotDisplay(cf90_addr, cf94_addr);
        }
        func_800451D0(MenuWidget_GetCurrentNode());
        return;
    }
    temp_s0_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    temp_s1_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xE);
    if ((temp_s0_2 != NULL) && (MenuWidget_GridCellIndex(temp_s0_2) != 0) && (temp_s1_2 != NULL)) {
        M2C_FIELD(temp_s0_2, s32 *, 0x44) = -1;
        func_80052E30(0);
        var_s0_2 = 0;
loop_16:
        if ((var_s0_2 < func_80052F70()) && (Inv_TestSelectionBit(var_s0_2) == 0)) {
            var_s0_2 += 1;
            goto loop_16;
        }
        M2C_FIELD(temp_s1_2, s32 *, 0x44) = 0;
        M2C_FIELD(temp_s1_2, s32 *, 0x48) = (s32) (var_s0_2 & -(var_s0_2 < func_80052F70()));
        MenuWidget_SetCurrentNode(temp_s1_2);
    }
}
