/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MenuWidget_FindByModeAndSelectedBase();
M2C_UNK MenuWidget_OffsetPosition();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK func_800525EC();
M2C_UNK func_800526C4();
s32 func_8005415C();
s32 Inv_GetPackedListCount();
M2C_UNK Inv_BuildFilteredPackedList();
M2C_UNK Inv_BuildFilteredPackedListExcluding();
s32 func_80059F08();
void *func_80062D2C();
void *MenuWidget_CreateNode();
M2C_UNK Draw_SetPrimCallback();
M2C_UNK func_80064B74();
M2C_UNK func_80064C20();
extern s32 g_MenuEquipMode;
extern s32 g_InvItemUsableFlag;
extern s32 g_MenuLayoutLocked;
extern struct { char _[16]; } func_800466C0_o __asm__("Menu_StepEquipConfirm");
#define Menu_StepEquipConfirm (*(M2C_UNK *)&func_800466C0_o)
extern struct { char _[16]; } func_8004FB48_o __asm__("Menu_DrawEquipSelectionList");
#define Menu_DrawEquipSelectionList (*(M2C_UNK *)&func_8004FB48_o)

void Menu_StepSkillList(s32 arg0, s32 arg1) {
    M2C_UNK var_a0_2;
    s32 saved_arg0 = arg0;
    s32 saved_arg1 = arg1;
    s32 temp_v0;
    s32 temp_shift;
    void *temp_s0;
    register void *temp_v0_2 asm("$17");
    register void *temp_v0_3 asm("$17");

    if (g_MenuLayoutLocked != 0) {
        temp_v0 = func_80059F08(0);
        if ((func_8005415C(temp_v0) != 0) && (func_8005415C(temp_v0) < 6)) {
            Inv_BuildFilteredPackedListExcluding(0x3E, temp_v0);
        } else {
            s32 mask;
            temp_shift = func_8005415C(temp_v0);
            asm volatile(
                "addiu $3, $0, 1\n"
                "sllv %0, $3, %1"
                : "=r"(mask)
                : "r"(temp_shift)
                : "$3");
            Inv_BuildFilteredPackedListExcluding(mask, temp_v0);
        }
        temp_v0_2 = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
        MenuWidget_OffsetPosition(temp_v0_2, 0xB4 - M2C_FIELD(temp_v0_2, s32 *, 0x18), 0xA4 - M2C_FIELD(temp_v0_2, s32 *, 0x1C));
    } else {
        var_a0_2 = 0x200;
        if (g_InvItemUsableFlag != 0) {
            var_a0_2 = 0x1FE;
        }
        Inv_BuildFilteredPackedList(var_a0_2);
    }
    if ((g_MenuEquipMode != 0) || (Inv_GetPackedListCount() != 0)) {
        temp_v0_3 = func_80062D2C(7, saved_arg0, 0, 0);
        temp_s0 = MenuWidget_CreateNode(7, temp_v0_3, temp_v0_3);
        M2C_FIELD(temp_v0_3, M2C_UNK **, 0x2C) = &Menu_StepEquipConfirm;
        {
            s32 temp_v1 = g_MenuLayoutLocked;
            M2C_FIELD(temp_s0, M2C_UNK **, 0x30) = &Menu_DrawEquipSelectionList;
            if (temp_v1 != 0) {
                func_80064C20(temp_s0);
            }
        }
        if (g_InvItemUsableFlag == 0) {
            func_80064B74(temp_s0, 0x13);
        }
        if (saved_arg1 == 0) {
            M2C_FIELD(temp_s0, s32 *, 0x44) = -1;
        } else {
            M2C_FIELD(temp_s0, s32 *, 0x44) = 0;
        }
        if (M2C_FIELD(temp_s0, s32 *, 0x48) < 0) {
            M2C_FIELD(temp_s0, s32 *, 0x48) = 0;
        }
        if (M2C_FIELD(temp_s0, s32 *, 0x44) >= 0) {
            MenuWidget_SetCurrentNode(temp_s0);
        }
        Draw_SetPrimCallback(temp_s0, Inv_GetPackedListCount());
        if (g_MenuEquipMode != 0) {
            M2C_FIELD(temp_s0, s32 *, 0x44) = -1;
            MenuWidget_OffsetPosition(temp_v0_3, 0, 0x14);
        }
        func_800525EC();
        return;
    }
    func_800526C4();
}
