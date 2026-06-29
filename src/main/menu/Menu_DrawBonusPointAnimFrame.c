/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Aya_UnlockParasiteSpellById();
M2C_UNK Draw_OffsetCursor();
M2C_UNK Draw_PrintNumberWidth5();
M2C_UNK Draw_SetColor();
M2C_UNK Menu_UpdateStatBarAnimation();
M2C_UNK Akao_FlushBgmVolumeFade();
M2C_UNK Draw_AllocSprite();
extern s32 g_BonusPointBarAnimProgress;
extern s32 g_MenuLevelDisplayValue;
extern s32 g_MenuHpMaxDisplayValue;
extern s32 g_BonusPointDisplayValue;
extern s32 g_MenuLevelDisplayTarget;
extern s32 g_MenuHpMaxDisplayTarget;
extern s32 g_MenuBonusPointDisplayTarget;
extern s32 g_MenuLevelUpAnimTimer;
extern s32 D_8009CF7C;
extern s32 g_MenuBonusPointBarAnimActive;
extern s32 g_BonusPointBarAnimStep;
extern struct { char _[16]; } D_800C0E06_o __asm__("g_AyaHpMax");
#define g_AyaHpMax (*(u16 *)&D_800C0E06_o)
extern struct { char _[16]; } g_AyaBonusPoints_o __asm__("g_AyaBonusPoints");
#define g_AyaBonusPoints (*(s32 *)&g_AyaBonusPoints_o)
extern struct { char _[16]; } g_AyaSaveLevel_o __asm__("g_AyaSaveLevel");
#define g_AyaSaveLevel (*(u8 *)&g_AyaSaveLevel_o)

void Menu_DrawBonusPointAnimFrame(void) {
    M2C_UNK var_a0;
    M2C_UNK var_a0_2;
    M2C_UNK var_a0_3;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_s0;
    s32 var_s0_2;

    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(0x97);
    var_s0 = 0;
    if (g_MenuLevelDisplayValue < g_MenuLevelDisplayTarget) {
        temp_v0 = g_MenuLevelUpAnimTimer < 0x1E;
        g_MenuLevelUpAnimTimer += 1;
        var_s0 = 1;
        if (temp_v0 == 0) {
            temp_a0 = g_MenuLevelDisplayValue + 1;
            g_MenuLevelUpAnimTimer = 0;
            g_MenuLevelDisplayValue = temp_a0;
            Aya_UnlockParasiteSpellById(temp_a0);
        }
    }
    var_a0 = 0x808080;
    if ((s32) g_AyaSaveLevel < g_MenuLevelDisplayValue) {
        var_a0 = 0x8080;
    }
    Draw_SetColor(var_a0);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuLevelDisplayValue + 1);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(-0x3C, 0x14);
    Draw_AllocSprite(0x99);
    if (g_MenuHpMaxDisplayValue < g_MenuHpMaxDisplayTarget) {
        temp_v1 = D_8009CF7C;
        D_8009CF7C = temp_v1 + 1;
        var_s0 = 1;
        if (temp_v1 >= 0) {
            D_8009CF7C = 0;
            g_MenuHpMaxDisplayValue += 1;
        }
    }
    var_a0_2 = 0x808080;
    if ((s32) g_AyaHpMax < g_MenuHpMaxDisplayValue) {
        var_a0_2 = 0x8080;
    }
    Draw_SetColor(var_a0_2);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuHpMaxDisplayValue);
    Draw_OffsetCursor(-0x3C, 0x13);
    Draw_SetColor(0x808080);
    Draw_AllocSprite(0x93);
    if (g_BonusPointDisplayValue < g_MenuBonusPointDisplayTarget) {
        g_BonusPointDisplayValue += 1;
        var_s0 = 1;
    }
    temp_v1 = g_BonusPointDisplayValue;
    var_a0_3 = 0x808080;
    if (g_AyaBonusPoints < temp_v1) {
        var_a0_3 = 0x8080;
    }
    Draw_SetColor(var_a0_3);
    Draw_OffsetCursor(0xF, 0x11);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
    Draw_OffsetCursor(0xA, -0x56);
    Draw_SetColor(0x808080);
    if (g_MenuBonusPointBarAnimActive != 0) {
        if (g_BonusPointBarAnimProgress < 0x80) {
            g_BonusPointBarAnimProgress += g_BonusPointBarAnimStep;
        }
        if ((var_s0 == 0) && (g_BonusPointBarAnimProgress >= 0x80)) {
            g_BonusPointBarAnimProgress = 0x80;
            g_MenuBonusPointBarAnimActive = 0;
        }
    }
    Draw_OffsetCursor(0x78, 2);
    var_s0_2 = 1;
    do {
        Menu_UpdateStatBarAnimation(var_s0_2);
        Draw_OffsetCursor(0, 0x10);
        var_s0_2 += 1;
    } while (var_s0_2 < 7);
    if (g_MenuBonusPointBarAnimActive == 0) {
        Akao_FlushBgmVolumeFade();
    }
}
