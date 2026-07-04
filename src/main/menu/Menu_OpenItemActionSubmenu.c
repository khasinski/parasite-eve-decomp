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

M2C_UNK Menu_StepEquipScreen();
M2C_UNK Menu_DrawContextActionList();
M2C_UNK Inv_SelectActiveList();
s32 Inv_GetActiveListItemType();
M2C_UNK MenuWidget_CreateSimpleNode();
M2C_UNK MenuWidget_CreateNode();
M2C_UNK MenuWidget_SetCurrentNode();
s32 Battle_IsInputAllowedWrapped();
s32 Inv_BuildCompatibleWeaponBitset();
s32 Battle_GetStateFlag1();
M2C_UNK Inv_RebuildSelectableMask();
M2C_UNK Inv_LookupActiveListData();
s32 Inv_TestSelectionBit();
extern s32 g_MenuItemActionContext;
extern s32 g_MenuActiveItemSlot;
extern s32 g_MenuItemActionDisabled;
extern s32 g_MenuEquipMode;
extern s32 g_MenuActiveItemList;

void Menu_OpenItemActionSubmenu(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_s0;
    s32 temp_s0;
    s32 temp_v0;
    s32 var_s0_2;
    s32 var_s2;
    void *temp_a0;
    void *temp_s0_2;
    void *temp_s1;
    void *temp_v0_2;

    g_MenuActiveItemList = arg1;
    Inv_SelectActiveList(arg1);
    g_MenuActiveItemSlot = arg2;
    temp_v0 = Inv_GetActiveListItemType(arg2);
    if (temp_v0 != 0xA) {
        if (((u32) (temp_v0 - 0xC) < 2U) || ((u32) (temp_v0 - 0xE) < 2U) || ((u32) (temp_v0 - 8) < 2U)) {
            g_MenuItemActionContext = 2;
        } else {
            g_MenuItemActionContext = 1;
        }
    } else {
        g_MenuItemActionContext = 0;
    }
    var_s0 = 2;
    if (g_MenuItemActionContext == var_s0) {
        var_s0 = 3;
    }
    temp_v0_2 = MenuWidget_CreateSimpleNode(var_s0, arg0, 0, 1);
    temp_s1 = MenuWidget_CreateNode(var_s0, temp_v0_2, temp_v0_2);
    temp_a0 = temp_s1;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0x2C) = &Menu_StepEquipScreen;
    M2C_FIELD(temp_s1, M2C_UNK **, 0x30) = &Menu_DrawContextActionList;
    MenuWidget_SetCurrentNode(temp_a0);
    var_s0_2 = 0;
    if ((g_MenuItemActionContext == 1) && ((Battle_IsInputAllowedWrapped() == 0) || (Inv_BuildCompatibleWeaponBitset(g_MenuActiveItemSlot) == 0) || (Battle_GetStateFlag1() != 0))) {
        var_s0_2 = 1;
    }
    g_MenuItemActionDisabled = var_s0_2;
    Inv_RebuildSelectableMask();
    if (g_MenuItemActionContext == 0) {
        var_s2 = 0;
        temp_s0 = g_MenuActiveItemSlot;
        temp_s0_2 = Inv_LookupActiveListData(temp_s0);
        if ((Inv_TestSelectionBit(temp_s0) != 0) && ((g_MenuEquipMode != 1) || (M2C_FIELD(temp_s0_2, u8 *, 6) != 0xA) || ((u8) M2C_FIELD(temp_s0_2, u8 *, 0xE) < 4U))) {
            var_s2 = 1;
        }
        if (var_s2 != 0) {
            goto block_21;
        }
        goto block_22;
    }
block_21:
    if (g_MenuItemActionDisabled != 0) {
block_22:
        M2C_FIELD(temp_s1, s32 *, 0x48) = 1;
    }
}
