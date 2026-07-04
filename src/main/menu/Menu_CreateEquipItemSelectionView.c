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
void *MenuWidget_CreateSimpleNode();
void *MenuWidget_CreateNode();
M2C_UNK MenuWidget_SetColumnLayout();
M2C_UNK MenuWidget_ClearColumnLayout();

extern s32 g_InvItemUsableFlag;
extern s32 g_MenuLayoutLocked;
extern M2C_UNK Menu_StepEquipSlotSelect[];
#define Menu_StepEquipSlotSelect (Menu_StepEquipSlotSelect[0])
extern M2C_UNK Menu_StepSkillScreen[];
#define Menu_StepSkillScreen (Menu_StepSkillScreen[0])
extern M2C_UNK Menu_DrawSoundTestList[];
#define Menu_DrawSoundTestList (Menu_DrawSoundTestList[0])
extern M2C_UNK Menu_DrawItemListInvPanel[];
#define Menu_DrawItemListInvPanel (Menu_DrawItemListInvPanel[0])

void Menu_CreateEquipItemSelectionView(s32 arg0);

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
        callback = &Menu_DrawSoundTestList;
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
