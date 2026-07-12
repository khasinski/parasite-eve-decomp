/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MenuWidget_FindByModeAndSelectedBase();
s32 MenuWidget_GetCurrentNode();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_CreateAmmoSpendPanel();
M2C_UNK Inv_SelectActiveList();
s32 func_80052F0C();
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
s32 Inv_TestSelectionBit();
M2C_UNK Inv_InitSlotDisplay(void *, void *);
s32 Inv_BuildCompatibleWeaponBitset();
s32 MenuWidget_GridCellIndex();
extern s32 g_InvAmmoSpendActiveList;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapSourceList;
extern M2C_UNK g_InvSwapTargetIndex[];
#define g_InvSwapTargetIndex (g_InvSwapTargetIndex[0])

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
            Inv_SelectActiveList(0);
            var_s0 = 0;
loop_7:
            if ((var_s0 < Inv_GetAyaSlotLimit()) && (Inv_TestSelectionBit(var_s0) == 0)) {
                var_s0 += 1;
                goto loop_7;
            }
            M2C_FIELD(temp_s1, s32 *, 0x44) = 0;
            M2C_FIELD(temp_s1, s32 *, 0x48) = (s32) (var_s0 & -(var_s0 < Inv_GetAyaSlotLimit()));
            MenuWidget_SetCurrentNode(temp_s1);
        }
        {
            register u8 *cf90_addr asm("$4");
            register u8 *cf94_addr asm("$5");
            s32 cf88_value;

            cf90_addr = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(cf90_addr) : "0"(cf90_addr));
            cf90_addr -= 0x3070;
            cf88_value = g_InvAmmoSpendActiveList;
            __asm__ volatile("" : "=r"(cf88_value) : "0"(cf88_value));
            cf94_addr = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(cf94_addr) : "0"(cf94_addr));
            cf94_addr -= 0x306C;
            g_InvSwapSourceList = cf88_value;
            __asm__ volatile("" : : "r"(cf90_addr), "r"(cf94_addr));
            Inv_InitSlotDisplay(cf90_addr, cf94_addr);
        }
        Menu_CreateAmmoSpendPanel(MenuWidget_GetCurrentNode());
        return;
    }
    temp_s0_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    temp_s1_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xE);
    if ((temp_s0_2 != NULL) && (MenuWidget_GridCellIndex(temp_s0_2) != 0) && (temp_s1_2 != NULL)) {
        M2C_FIELD(temp_s0_2, s32 *, 0x44) = -1;
        Inv_SelectActiveList(0);
        var_s0_2 = 0;
loop_16:
        if ((var_s0_2 < Inv_GetAyaSlotLimit()) && (Inv_TestSelectionBit(var_s0_2) == 0)) {
            var_s0_2 += 1;
            goto loop_16;
        }
        M2C_FIELD(temp_s1_2, s32 *, 0x44) = 0;
        M2C_FIELD(temp_s1_2, s32 *, 0x48) = (s32) (var_s0_2 & -(var_s0_2 < Inv_GetAyaSlotLimit()));
        MenuWidget_SetCurrentNode(temp_s1_2);
    }
}
