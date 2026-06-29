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

M2C_UNK MenuWidget_SetCurrentNode();
void *MenuWidget_CreateSimpleNode() __asm__("func_80062D2C");
void *MenuWidget_CreateNode();
M2C_UNK MenuWidget_SetColumnLayout();
M2C_UNK MenuWidget_ClearColumnLayout() __asm__("func_80064C20");

extern s32 g_InvItemUsableFlag;
extern s32 g_MenuLayoutLocked;
extern struct { char _[16]; } func_80045A98_o __asm__("Menu_StepEquipSlotSelect");
#define Menu_StepEquipSlotSelect (*(M2C_UNK *)&func_80045A98_o)
extern struct { char _[16]; } func_80045D0C_o __asm__("Menu_StepSkillScreen");
#define Menu_StepSkillScreen (*(M2C_UNK *)&func_80045D0C_o)
extern struct { char _[16]; } func_8004F978_o __asm__("func_8004F978");
#define func_8004F978 (*(M2C_UNK *)&func_8004F978_o)
extern struct { char _[16]; } func_8004FFD0_o __asm__("Menu_DrawItemListInvPanel");
#define Menu_DrawItemListInvPanel (*(M2C_UNK *)&func_8004FFD0_o)

asm(".globl Menu_CreateEquipItemSelectionView");
asm("Menu_CreateEquipItemSelectionView = func_8004542C");

void Menu_CreateEquipItemSelectionView(s32 arg0) __asm__("func_8004542C");

void Menu_CreateEquipItemSelectionView(s32 arg0) {
    void *temp_s0;
    void *temp_v0;
    void *temp_v0_2;

    temp_v0 = MenuWidget_CreateSimpleNode(5, arg0, 0, 0);
    temp_s0 = MenuWidget_CreateNode(5, temp_v0, temp_v0);
    {
        M2C_UNK *callback;
        register s32 flag asm("$3");

        flag = g_MenuLayoutLocked;
        callback = &Menu_StepEquipSlotSelect;
        M2C_FIELD(temp_v0, M2C_UNK **, 0x30) = callback;
        callback = &Menu_StepSkillScreen;
        M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = callback;
        callback = &func_8004F978;
        M2C_FIELD(temp_s0, M2C_UNK **, 0x30) = callback;
        if (flag != 0) {
            MenuWidget_ClearColumnLayout(temp_s0);
        } else if (g_InvItemUsableFlag == 0) {
            MenuWidget_SetColumnLayout(temp_s0, 0x12);
        }
    }
    if (M2C_FIELD(temp_s0, s32 *, 0x44) >= 0) {
        MenuWidget_SetCurrentNode(temp_s0);
    }
    M2C_FIELD(temp_v0, s32 *, 0x40) = 1;
    temp_s0 = MenuWidget_CreateNode(0x1B, temp_v0, temp_v0);
    temp_v0_2 = temp_s0;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0x30) = &Menu_DrawItemListInvPanel;
    MenuWidget_ClearColumnLayout(temp_v0_2);
}
