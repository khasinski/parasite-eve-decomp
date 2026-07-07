/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Save_GetActiveMetadataBuffer();                 /* extern */
M2C_UNK Sfx_DrawActiveListSlot(s8);                          /* extern */
s32 Aya_GetLevelExpTable();                                /* extern */
M2C_UNK Draw_OffsetCursor(M2C_UNK, M2C_UNK);            /* extern */
M2C_UNK Draw_AllocSprite(M2C_UNK);                     /* extern */
M2C_UNK Draw_PrintRawText(s32);                         /* extern */
M2C_UNK Draw_PrintTextById(M2C_UNK);                     /* extern */
M2C_UNK Draw_PrintNumberWidth6(s32);                         /* extern */
M2C_UNK Draw_PrintNumberWidth3(s32);                         /* extern */
extern s8 g_AyaEquippedWeaponSlot[];
#define g_AyaEquippedWeaponSlot (g_AyaEquippedWeaponSlot[0])
extern s8 g_AyaEquippedArmorSlot[];
#define g_AyaEquippedArmorSlot (g_AyaEquippedArmorSlot[0])
extern u8 g_AyaSaveLevel[];
#define g_AyaSaveLevel (g_AyaSaveLevel[0])
extern s32 g_AyaSaveTotalExp[];
#define g_AyaSaveTotalExp (g_AyaSaveTotalExp[0])

void Menu_DrawStatusPanel(void) {
    s32 var_a0;

    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x47);
    Draw_OffsetCursor(0x28, 2);
    Draw_PrintRawText(Save_GetActiveMetadataBuffer());
    Draw_OffsetCursor(0, 0x15);
    Draw_AllocSprite(0x97);
    Draw_OffsetCursor(0x3C, 0);
    Draw_PrintNumberWidth3(g_AyaSaveLevel + 1);
    Draw_OffsetCursor(-0x78, 0x1A);
    Draw_AllocSprite(0x98);
    Draw_OffsetCursor(0x42, 0);
    if ((u8) g_AyaSaveLevel < 0x62U) {
        var_a0 = M2C_FIELD(((g_AyaSaveLevel * 4) + Aya_GetLevelExpTable()), s32 *, 4) - g_AyaSaveTotalExp;
    } else {
        var_a0 = 0;
    }
    Draw_PrintNumberWidth6(var_a0);
    Draw_OffsetCursor(-0x7C, 0x15);
    Draw_AllocSprite(0x94);
    Draw_OffsetCursor(0, 0x10);
    Sfx_DrawActiveListSlot(g_AyaEquippedWeaponSlot);
    Draw_OffsetCursor(0, 0x10);
    if (g_AyaEquippedArmorSlot >= 0) {
        Sfx_DrawActiveListSlot(g_AyaEquippedArmorSlot);
        return;
    }
    Draw_PrintTextById(0x39);
}
