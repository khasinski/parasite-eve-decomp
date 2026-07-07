/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void Draw_OffsetCursor(int, int);
M2C_UNK Draw_AllocSprite();
extern u8 D_800922B8[];
extern u8 D_800922BC[];
extern u8 D_800922C4[];
extern u8 D_800922CC[];
extern s32 g_MenuInventoryViewMode;

void Menu_DrawWeaponModSlots(s32 arg0) {
    s32 temp_s0;
    u8 var_a0;

    temp_s0 = arg0 + D_800922B8[g_MenuInventoryViewMode];
    if (g_MenuInventoryViewMode == 1) {
        Draw_OffsetCursor(0, 2);
        Draw_AllocSprite(D_800922BC[temp_s0]);
        Draw_OffsetCursor(0x12, 2);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -2);
        Draw_AllocSprite(D_800922C4[temp_s0]);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 2);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -2);
            var_a0 = D_800922CC[temp_s0];
            goto block_5;
        }
    } else {
        Draw_AllocSprite(D_800922BC[temp_s0]);
        Draw_AllocSprite(0x68U);
        Draw_OffsetCursor(0x12, 4);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -4);
        Draw_AllocSprite(D_800922C4[temp_s0]);
        Draw_AllocSprite(0x68U);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 4);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -4);
            Draw_AllocSprite(D_800922CC[temp_s0]);
            var_a0 = 0x68;
block_5:
            Draw_AllocSprite(var_a0);
        }
    }
}
