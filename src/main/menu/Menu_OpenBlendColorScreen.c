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

M2C_UNK MenuWidget_CreateSimpleNode();
M2C_UNK MenuWidget_CreateNode();
M2C_UNK MenuWidget_SetCurrentNode();
s32 Draw_GetBlendColor();
M2C_UNK Menu_DrawBlendColorChannelListUnk();
M2C_UNK Menu_StepColorSelect();
M2C_UNK Menu_DrawBlendColorChannelList();
M2C_UNK Menu_DrawBlendColorOptionList();
extern u8 D_800922D4[];
extern s32 D_8009CFE0;

void Menu_OpenBlendColorScreen(s32 arg0) {
    void *temp_v0;
    register void *temp_v0_2 asm("$16");
    void *temp_v0_3;
    void *var_a0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x2E, arg0, 0, 0);
    temp_v0_2 = MenuWidget_CreateNode(0x2E, temp_v0, temp_v0);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x30) = &Menu_DrawBlendColorChannelListUnk;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_StepColorSelect;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x4C) = &D_800922D4;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 1;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0x30) = &Menu_DrawBlendColorChannelList;
    M2C_FIELD(temp_v0_2, s32 *, 0x28) = 1;
    temp_v0_3 = MenuWidget_CreateNode(0x31, temp_v0, temp_v0);
    M2C_FIELD(temp_v0_3, M2C_UNK **, 0x30) = &Menu_DrawBlendColorOptionList;
    M2C_FIELD(temp_v0_2, void **, 0x7C) = temp_v0_3;
    M2C_FIELD(temp_v0_3, void **, 0x78) = temp_v0_2;
    var_a0 = temp_v0_2;
    if (M2C_FIELD(temp_v0_2, s32 *, 0x44) < 0) {
        var_a0 = temp_v0_3;
    }
    MenuWidget_SetCurrentNode(var_a0);
    D_8009CFE0 = Draw_GetBlendColor();
}
