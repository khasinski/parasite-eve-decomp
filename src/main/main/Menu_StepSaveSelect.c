/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
u8 *MemCard_GetSlot();
s32 MenuWidget_FindByModeAndSelectedBase();
s32 MenuWidget_GetCellIndex();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Save_SetTitleStyleFlag();
s32 func_80042964();
M2C_UNK MemCard_CloseAllAndResetState();
M2C_UNK func_80042B50();
M2C_UNK Menu_CreateTwoLineDialog();
M2C_UNK Menu_CreateMemCardProgressWidget();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
M2C_UNK Menu_PlayErrorSound();
M2C_UNK Util_AppendFFTerminatedBytes();
M2C_UNK Inv_SelectActiveList();
s32 Str_LookupTable4();
s32 Draw_MeasureTextWidth();
s32 MenuWidget_GetChild(s32, s32);
void *MenuWidget_CreateSimpleNode();
M2C_UNK MenuWidget_DestroyNode();
M2C_UNK MenuWidget_NavScrollTo();
void *MenuWidget_CreateNode();
extern s32 g_MenuActiveItemList;
extern s32 g_MenuActionTextBase;
extern s32 g_SaveSelectedSlot;
extern s32 g_SaveSlotCellIndex;
extern s32 D_8009CF4C;
extern s32 g_McDialogMode;
extern s32 g_MenuItemPrimListHandles;
extern M2C_UNK *g_MenuConfirmCallback;
extern s32 g_MenuMemCardConfirmPending;
extern struct { char _[16]; } D_800A1980_o __asm__("g_MenuItemLabelBuffer");
#define g_MenuItemLabelBuffer (*(u8 *)&D_800A1980_o)
extern struct { char _[16]; } Menu_DrawItemLabel_o __asm__("Menu_DrawItemLabel");
#define Menu_DrawItemLabel (*(M2C_UNK *)&Menu_DrawItemLabel_o)
extern struct { char _[16]; } func_80044E98_o __asm__("Menu_ConfirmDialogHandler");
#define Menu_ConfirmDialogHandler (*(M2C_UNK *)&func_80044E98_o)
extern struct { char _[16]; } func_8004F950_o __asm__("Menu_DrawActionOptionList");
#define Menu_DrawActionOptionList (*(M2C_UNK *)&func_8004F950_o)
extern struct { char _[16]; } func_800504F4_o __asm__("Save_StartSelectedWriteSlot");
#define func_800504F4 (*(M2C_UNK *)&func_800504F4_o)
extern struct { char _[16]; } func_8005051C_o __asm__("Save_StartSelectedReadSlot");
#define func_8005051C (*(M2C_UNK *)&func_8005051C_o)
extern struct { char _[16]; } func_80050544_o __asm__("Menu_TriggerSaveWrite");
#define Menu_TriggerSaveWrite (*(M2C_UNK *)&func_80050544_o)

s32 Menu_StepSaveSelect(s32 arg0, s32 arg1) {
    M2C_UNK *var_a0;
    s32 temp_s0;
    s32 temp_s2;
    s32 temp_v0;
    register s32 temp_cf50 asm("$5");
    s32 temp_height;
    register s32 var_v1 asm("$3");
    s32 temp_a0;
    u8 *temp_v0_2;
    void *temp_s2_2;
    void *temp_v0_3;

    temp_s2 = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x40) {
        MenuWidget_NavScrollTo(0x3F);
        MenuWidget_DestroyNode(arg0);
        MemCard_CloseAllAndResetState();
        Menu_PlayCancelSound();
        return 1;
    }
    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetCellIndex(temp_s2);
        g_SaveSlotCellIndex = temp_v0;
        if (temp_v0 >= 0) {
            temp_v0_2 = MemCard_GetSlot(g_SaveSelectedSlot, temp_v0);
            if (temp_v0_2 != NULL) {
                temp_cf50 = g_McDialogMode;
                if ((temp_cf50 != 0) || (*temp_v0_2 != 2)) {
                    temp_v0 = *temp_v0_2;
                    asm volatile("nop");
                    D_8009CF4C = temp_v0;
                    if (temp_cf50 != 0) {
                        Save_SetTitleStyleFlag(g_MenuMemCardConfirmPending, temp_cf50);
                        if (D_8009CF4C != 2) {
                            temp_v0_3 = MenuWidget_CreateSimpleNode(0x29, temp_s2, 0, 1);
                            temp_s2_2 = MenuWidget_CreateNode(0x29, temp_v0_3, temp_v0_3);
                            M2C_FIELD(temp_v0_3, M2C_UNK **, 0x30) = &Menu_DrawItemLabel;
                            M2C_FIELD(temp_v0_3, M2C_UNK **, 0x2C) = &Menu_ConfirmDialogHandler;
                            M2C_FIELD(temp_s2_2, M2C_UNK **, 0x30) = &Menu_DrawActionOptionList;
                            g_MenuActionTextBase = 0x6C;
                            MenuWidget_SetCurrentNode(temp_s2_2);
                            temp_a0 = g_MenuActiveItemList;
                            M2C_FIELD(temp_s2_2, s32 *, 0x44) = 1;
                            Inv_SelectActiveList(temp_a0);
                            g_MenuItemLabelBuffer = 0xFF;
                            Util_AppendFFTerminatedBytes(&g_MenuItemLabelBuffer, Str_LookupTable4(0x44));
                            g_MenuItemPrimListHandles = 0;
                            {
                                M2C_UNK *callback = &Menu_TriggerSaveWrite;
                                var_v1 = 0x78;
                                if (Draw_MeasureTextWidth(&g_MenuItemLabelBuffer) < 0x78) {
                                    var_v1 = 0x78;
                                } else {
                                    var_v1 = Draw_MeasureTextWidth(&g_MenuItemLabelBuffer);
                                }
                                M2C_FIELD(temp_v0_3, s32 *, 0x34) = (s32) (var_v1 + 0x14);
                                M2C_FIELD(temp_v0_3, s32 *, 0x38) = 0x32;
                                M2C_FIELD(temp_v0_3, s32 *, 0x18) = (s32) ((s32) (0x12C - var_v1) >> 1);
                                M2C_FIELD(temp_s2_2, s32 *, 0x18) = (s32) ((s32) (M2C_FIELD(temp_v0_3, s32 *, 0x34) - 0x80) >> 1);
                                temp_height = M2C_FIELD(temp_v0_3, s32 *, 0x38);
                                g_MenuConfirmCallback = callback;
                                M2C_FIELD(temp_s2_2, s32 *, 0x1C) = (s32) (temp_height - 0x14);
                            }
                        } else {
                            if (func_80042964(g_SaveSelectedSlot) < 0xF) {
                                MenuWidget_NavScrollTo(0x1F);
                                Menu_CreateMemCardProgressWidget(0x45);
                                var_a0 = &func_800504F4;
                                goto block_17;
                            }
                            temp_s0 = g_SaveSelectedSlot;
                            if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
                                Menu_CreateTwoLineDialog(temp_s0 + 0x47, g_McDialogMode + 0x42);
                            }
                        }
                    } else {
                        MenuWidget_NavScrollTo(0x1F);
                        Menu_CreateMemCardProgressWidget(0x46);
                        var_a0 = &func_8005051C;
block_17:
                        func_80042B50(var_a0);
                    }
                    Menu_PlayConfirmSound();
                    return 1;
                }
            }
        }
        Menu_PlayErrorSound();
        /* Duplicate return node #20. Try simplifying control flow for better match */
        return 1;
    }
    return 1;
}
