/* CC1_FLAGS: -g3 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Str_LookupTable8();
M2C_UNK Sfx_DrawSlotRow();
M2C_UNK Draw_OffsetCursor();
extern u8 g_InvCompareSlotLeft[];
#define g_InvCompareSlotLeft (g_InvCompareSlotLeft[0])
extern u8 D_800A1F98[];
#define D_800A1F98 (D_800A1F98[0])
extern u8 D_800A1F99[];
#define D_800A1F99 (D_800A1F99[0])
extern u8 D_800A1F9A[];
#define D_800A1F9A (D_800A1F9A[0])
extern u8 g_InvCompareSlotRight[];
#define g_InvCompareSlotRight (g_InvCompareSlotRight[0])
extern u8 D_800A1FB8[];
#define D_800A1FB8 (D_800A1FB8[0])
extern u8 D_800A1FB9[];
#define D_800A1FB9 (D_800A1FB9[0])
extern u8 D_800A1FBA[];
#define D_800A1FBA (D_800A1FBA[0])
extern u8 g_CursorRenderDataBlock[];
#define g_CursorRenderDataBlock (g_CursorRenderDataBlock[0])
void Menu_DrawWeaponComparisonPanel(void) {
    M2C_UNK *var_a1;
    M2C_UNK *var_a1_2;

    if (D_800A1F99 & 0x10) {
        var_a1 = &g_CursorRenderDataBlock;
        if (D_800A1F9A == 9) {
            var_a1 = &g_CursorRenderDataBlock + 0x10;
        }
    } else {
        var_a1 = Str_LookupTable8(D_800A1F98 - 1);
    }
    Sfx_DrawSlotRow(&g_InvCompareSlotLeft, var_a1);
    Draw_OffsetCursor(0, 0x18);
    if (D_800A1FB9 & 0x10) {
        var_a1_2 = &g_CursorRenderDataBlock;
        if (D_800A1FBA == 9) {
            var_a1_2 = &g_CursorRenderDataBlock + 0x10;
        }
    } else {
        var_a1_2 = Str_LookupTable8(D_800A1FB8 - 1);
    }
    Sfx_DrawSlotRow(&g_InvCompareSlotRight, var_a1_2);
}
