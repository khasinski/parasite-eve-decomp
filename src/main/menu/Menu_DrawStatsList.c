/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Battle_GetModifierTable();
M2C_UNK Stat_QueryLevelAndSubLevel();
M2C_UNK func_8005E8A4();
M2C_UNK Draw_AllocSprite();
M2C_UNK func_8005F5B8();
M2C_UNK func_800605F8();
extern s32 g_BonusPointBarAnimProgress;
extern struct { char _[16]; } D_800A18DC_o __asm__("D_800A18DC");
#define D_800A18DC (*(s32 *)&D_800A18DC_o)
extern struct { char _[16]; } D_800A18FC_o __asm__("g_BonusPointStatMultipliers");
#define g_BonusPointStatMultipliers (*(M2C_UNK *)&D_800A18FC_o)

void Menu_DrawStatsList(void) {
    s32 sp10;
    M2C_UNK var_a0;
    M2C_UNK var_a1;
    u8 *var_s3;
    register s32 *var_s4 asm("$20");
    s32 temp_s0;
    s32 temp_s1;
    register s32 var_a0_2 asm("$4");
    s32 var_s2;

    var_s4 = Battle_GetModifierTable() + 4;
    func_8005E8A4(4, 5);
    var_s2 = 1;
    var_s3 = &D_800A18DC;
    do {
        temp_s1 = *var_s4;
        temp_s0 = M2C_FIELD(var_s3, s32 *, 0) + ((s32) (g_BonusPointBarAnimProgress * M2C_FIELD(&g_BonusPointStatMultipliers, s32 *, var_s2 * 4)) >> 7);
        func_8005E8A4(2, 0);
        Draw_AllocSprite(var_s2 + 0x8C);
        func_8005E8A4(0x4A, 0);
        Stat_QueryLevelAndSubLevel(var_s2, temp_s0, &sp10, 0);
        var_s4 += 1;
        func_800605F8(sp10 + 1);
        if (temp_s1 != 0) {
            func_8005E8A4(6, 0);
            var_a0 = 0x70;
            if (temp_s1 > 0) {
                var_a0 = 0x6F;
            }
            func_8005F5B8(var_a0);
            var_a0_2 = temp_s1;
            if (temp_s1 < 0) {
                var_a0_2 = -var_a0_2;
            }
            func_800605F8(var_a0_2);
            func_8005E8A4(-0x18, 0);
        }
        var_a1 = 0xE;
        if (var_s2 == 4) {
            var_a1 = 0x16;
        }
        func_8005E8A4(-0x5E, var_a1);
        var_s2 += 1;
        var_s3 += 4;
    } while (var_s2 < 7);
}
