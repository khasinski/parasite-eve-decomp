/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuInput_GetStatusFlags();
M2C_UNK Draw_OffsetCursor();
M2C_UNK Draw_SetTextDimmed();
M2C_UNK Draw_AllocSprite();
s32 MenuWidget_GetChild();
s32 MenuWidget_GridCellIndex();

void Menu_DrawBlendColorChannelList(s32 arg0) {
    M2C_UNK var_a0;
    M2C_UNK var_a0_2;
    M2C_UNK var_a0_3;
    M2C_UNK var_a0_4;
    M2C_UNK var_a0_5;
    M2C_UNK var_a0_6;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s1;

    temp_s1 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
    temp_s0 = MenuInput_GetStatusFlags() & 0x1000;
    asm volatile("" : "=r"(temp_s0) : "0"(temp_s0));
    Draw_OffsetCursor(0x12, 5);
    var_a0 = 0;
    if ((temp_s1 != 0) || (temp_s0 == 0)) {
        var_a0 = 1;
    }
    Draw_SetTextDimmed(var_a0);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    var_a0_2 = 0;
    if ((temp_s1 != 1) || (temp_s0 == 0)) {
        var_a0_2 = 1;
    }
    Draw_SetTextDimmed(var_a0_2);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    var_a0_3 = 0;
    if ((temp_s1 != 2) || (temp_s0 == 0)) {
        var_a0_3 = 1;
    }
    Draw_SetTextDimmed(var_a0_3);
    Draw_AllocSprite(0x4A);
    temp_s0_2 = MenuInput_GetStatusFlags() & 0x4000;
    asm volatile("" : "=r"(temp_s0_2) : "0"(temp_s0_2));
    Draw_OffsetCursor(-0x40, 0x19);
    var_a0_4 = 0;
    if ((temp_s1 != 0) || (temp_s0_2 == 0)) {
        var_a0_4 = 1;
    }
    Draw_SetTextDimmed(var_a0_4);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    var_a0_5 = 0;
    if ((temp_s1 != 1) || (temp_s0_2 == 0)) {
        var_a0_5 = 1;
    }
    Draw_SetTextDimmed(var_a0_5);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    var_a0_6 = 0;
    if ((temp_s1 != 2) || (temp_s0_2 == 0)) {
        var_a0_6 = 1;
    }
    Draw_SetTextDimmed(var_a0_6);
    Draw_AllocSprite(0x4B);
}
