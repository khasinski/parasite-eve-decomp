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
M2C_UNK Str_LookupTable8();
M2C_UNK Sfx_DrawSlotRow();
M2C_UNK Draw_OffsetCursor();
extern struct { char _[16]; } D_800A1F94_o __asm__("g_InvCompareSlotLeft");
#define g_InvCompareSlotLeft (*(u8 *)&D_800A1F94_o)
extern struct { char _[16]; } D_800A1F98_o __asm__("D_800A1F98");
#define D_800A1F98 (*(u8 *)&D_800A1F98_o)
extern struct { char _[16]; } D_800A1F99_o __asm__("D_800A1F99");
#define D_800A1F99 (*(u8 *)&D_800A1F99_o)
extern struct { char _[16]; } D_800A1F9A_o __asm__("D_800A1F9A");
#define D_800A1F9A (*(u8 *)&D_800A1F9A_o)
extern struct { char _[16]; } D_800A1FB4_o __asm__("g_InvCompareSlotRight");
#define g_InvCompareSlotRight (*(u8 *)&D_800A1FB4_o)
extern struct { char _[16]; } D_800A1FB8_o __asm__("D_800A1FB8");
#define D_800A1FB8 (*(u8 *)&D_800A1FB8_o)
extern struct { char _[16]; } D_800A1FB9_o __asm__("D_800A1FB9");
#define D_800A1FB9 (*(u8 *)&D_800A1FB9_o)
extern struct { char _[16]; } D_800A1FBA_o __asm__("D_800A1FBA");
#define D_800A1FBA (*(u8 *)&D_800A1FBA_o)
extern struct { char _[16]; } D_800C20A4_o __asm__("g_CursorRenderDataBlock");
#define g_CursorRenderDataBlock (*(u8 *)&D_800C20A4_o)
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
