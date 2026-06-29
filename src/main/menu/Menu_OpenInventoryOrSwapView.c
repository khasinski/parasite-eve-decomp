/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_ClearCursorY();
s32 MenuWidget_FindByModeAndSelectedBase();
void *MenuWidget_GetCurrentNode();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK func_800439D8();
s32 func_80052F70();
M2C_UNK Inv_RebuildSelectableMask();
void *func_80062D2C();
M2C_UNK MenuWidget_NavScrollTo();
void *MenuWidget_CreateNode();
M2C_UNK Draw_SetPrimCallback();
extern s32 g_MenuActionSubmenuOpen;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuEquipSwapSource;
extern s32 g_MenuInventoryViewMode;
extern struct { char _[16]; } func_80044444_o __asm__("Menu_InventoryInputHandler");
#define Menu_InventoryInputHandler (*(M2C_UNK *)&func_80044444_o)
extern struct { char _[16]; } func_800447F0_o __asm__("Menu_DrawAmmoTypeHeader");
#define Menu_DrawAmmoTypeHeader (*(M2C_UNK *)&func_800447F0_o)
extern struct { char _[16]; } func_8004F8D0_o __asm__("func_8004F8D0");
#define func_8004F8D0 (*(M2C_UNK *)&func_8004F8D0_o)
extern struct { char _[16]; } func_80050260_o __asm__("Menu_RebuildSelectableMask");
#define Menu_RebuildSelectableMask (*(M2C_UNK *)&func_80050260_o)
extern struct { char _[16]; } Inv_SwapSlots_o __asm__("Inv_SwapSlots");
#define Inv_SwapSlots (*(M2C_UNK *)&Inv_SwapSlots_o)

void Menu_OpenInventoryOrSwapView(s32 arg0) {
    s32 mode;
    register s32 temp_v1 asm("$3");
    void *temp_a0;
    void *temp_s1;
    void *temp_v0;
    void *temp_v0_2;

    MenuWidget_NavScrollTo(0x3C);
    MenuWidget_NavScrollTo(0x3B);
    MenuWidget_NavScrollTo(0x3A);
    mode = g_MenuInventoryViewMode;
    if (mode == 1) {
        goto clear_cursor;
    }
    if (mode >= 2) {
        goto check_two;
    }
    if (mode == 0) {
        goto case_zero;
    }
    return;
check_two:
    if (mode == 2) {
        goto clear_cursor;
    }
    return;
case_zero:
    if (arg0 != 0) {
        temp_v0 = MenuWidget_GetCurrentNode();
        M2C_FIELD(temp_v0, s32 *, 0x48) = 0;
        M2C_FIELD(func_80062D2C(0x1B, NULL, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawAmmoTypeHeader;
        temp_v0_2 = func_80062D2C(1, temp_v0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(1, temp_v0_2, temp_v0_2);
        temp_a0 = temp_s1;
        M2C_FIELD(temp_v0_2, M2C_UNK **, 0x2C) = &Menu_InventoryInputHandler;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x30) = &func_8004F8D0;
        MenuWidget_SetCurrentNode(temp_a0);
        M2C_FIELD(temp_s1, M2C_UNK **, 0x84) = &Inv_SwapSlots;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x88) = &Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(temp_s1, func_80052F70());
        temp_v1 = g_MenuEquipSwapSource;
        g_MenuActionSubmenuOpen = 0;
        if (temp_v1 != 0) {
            temp_v1 -= 1;
            M2C_FIELD(temp_s1, s32 *, 0x44) = (s32) (temp_v1 & 1);
            M2C_FIELD(temp_s1, s32 *, 0x48) = (s32) ((temp_v1 >> 1) & 0x7F);
            M2C_FIELD(temp_s1, s32 *, 0x5C) = (s32) (temp_v1 >> 8);
            return;
        }
        return;
    }
    func_800439D8();
    return;
clear_cursor:
    MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));
}
