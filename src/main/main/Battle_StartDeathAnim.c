/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Entity_SetActionMode();
M2C_UNK Battle_ResetEnemyStats();
M2C_UNK func_8006DE80();
extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_b __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(u8 **)&g_PlayerEntity_o)
#define D254_B (*(u8 **)&g_PlayerEntity_b)
extern s32 g_BattleModeState;
extern s32 g_FieldMoveLock_r[] __asm__("g_FieldMoveLock");
extern s32 g_FieldMoveLock_w[] __asm__("g_FieldMoveLock");
extern struct { char _[16]; } D_800B00EC_o __asm__("D_800B00EC");
#define D_800B00EC (*(s8 *)&D_800B00EC_o)
extern struct { char _[16]; } D_800B00ED_o __asm__("D_800B00ED");
#define D_800B00ED (*(s8 *)&D_800B00ED_o)
extern struct { char _[16]; } D_800B00EE_o __asm__("D_800B00EE");
#define D_800B00EE (*(s8 *)&D_800B00EE_o)
extern struct { char _[16]; } D_800B00F4_o __asm__("D_800B00F4");
#define D_800B00F4 (*(s8 *)&D_800B00F4_o)
extern struct { char _[16]; } D_800B00F5_o __asm__("D_800B00F5");
#define D_800B00F5 (*(s8 *)&D_800B00F5_o)
extern struct { char _[16]; } D_800B00F6_o __asm__("D_800B00F6");
#define D_800B00F6 (*(s8 *)&D_800B00F6_o)
extern struct { char _[16]; } D_800B00FC_o __asm__("D_800B00FC");
#define D_800B00FC (*(s8 *)&D_800B00FC_o)
extern struct { char _[16]; } D_800B00FD_o __asm__("D_800B00FD");
#define D_800B00FD (*(s8 *)&D_800B00FD_o)
extern struct { char _[16]; } D_800B00FE_o __asm__("D_800B00FE");
#define D_800B00FE (*(s8 *)&D_800B00FE_o)
extern struct { char _[16]; } D_800B0104_o __asm__("D_800B0104");
#define D_800B0104 (*(s8 *)&D_800B0104_o)
extern struct { char _[16]; } D_800B0105_o __asm__("D_800B0105");
#define D_800B0105 (*(s8 *)&D_800B0105_o)
extern struct { char _[16]; } D_800B0106_o __asm__("D_800B0106");
#define D_800B0106 (*(s8 *)&D_800B0106_o)
extern struct { char _[16]; } D_800B0110_o __asm__("D_800B0110");
#define D_800B0110 (*(s8 *)&D_800B0110_o)
extern struct { char _[16]; } D_800B0111_o __asm__("D_800B0111");
#define D_800B0111 (*(s8 *)&D_800B0111_o)
extern struct { char _[16]; } D_800B0112_o __asm__("D_800B0112");
#define D_800B0112 (*(s8 *)&D_800B0112_o)
extern struct { char _[16]; } D_800B0118_o __asm__("D_800B0118");
#define D_800B0118 (*(s8 *)&D_800B0118_o)
extern struct { char _[16]; } D_800B0119_o __asm__("D_800B0119");
#define D_800B0119 (*(s8 *)&D_800B0119_o)
extern struct { char _[16]; } D_800B011A_o __asm__("D_800B011A");
#define D_800B011A (*(s8 *)&D_800B011A_o)
extern struct { char _[16]; } D_800B0120_o __asm__("D_800B0120");
#define D_800B0120 (*(s8 *)&D_800B0120_o)
extern struct { char _[16]; } D_800B0121_o __asm__("D_800B0121");
#define D_800B0121 (*(s8 *)&D_800B0121_o)
extern struct { char _[16]; } D_800B0122_o __asm__("D_800B0122");
#define D_800B0122 (*(s8 *)&D_800B0122_o)
extern struct { char _[16]; } D_800B0128_o __asm__("D_800B0128");
#define D_800B0128 (*(s8 *)&D_800B0128_o)
extern struct { char _[16]; } D_800B0129_o __asm__("D_800B0129");
#define D_800B0129 (*(s8 *)&D_800B0129_o)
extern struct { char _[16]; } D_800B012A_o __asm__("D_800B012A");
#define D_800B012A (*(s8 *)&D_800B012A_o)
extern struct { char _[16]; } D_800B0134_o __asm__("D_800B0134");
#define D_800B0134 (*(s8 *)&D_800B0134_o)
extern struct { char _[16]; } D_800B0135_o __asm__("D_800B0135");
#define D_800B0135 (*(s8 *)&D_800B0135_o)
extern struct { char _[16]; } D_800B0136_o __asm__("D_800B0136");
#define D_800B0136 (*(s8 *)&D_800B0136_o)
extern struct { char _[16]; } D_800B013C_o __asm__("D_800B013C");
#define D_800B013C (*(s8 *)&D_800B013C_o)
extern struct { char _[16]; } D_800B013D_o __asm__("D_800B013D");
#define D_800B013D (*(s8 *)&D_800B013D_o)
extern struct { char _[16]; } D_800B013E_o __asm__("D_800B013E");
#define D_800B013E (*(s8 *)&D_800B013E_o)
extern struct { char _[16]; } D_800B0144_o __asm__("D_800B0144");
#define D_800B0144 (*(s8 *)&D_800B0144_o)
extern struct { char _[16]; } D_800B0145_o __asm__("D_800B0145");
#define D_800B0145 (*(s8 *)&D_800B0145_o)
extern struct { char _[16]; } D_800B0146_o __asm__("D_800B0146");
#define D_800B0146 (*(s8 *)&D_800B0146_o)
extern struct { char _[16]; } D_800B014C_o __asm__("D_800B014C");
#define D_800B014C (*(s8 *)&D_800B014C_o)
extern struct { char _[16]; } D_800B014D_o __asm__("D_800B014D");
#define D_800B014D (*(s8 *)&D_800B014D_o)
extern struct { char _[16]; } D_800B014E_o __asm__("D_800B014E");
#define D_800B014E (*(s8 *)&D_800B014E_o)
extern struct { char _[16]; } D_800B0158_o __asm__("D_800B0158");
#define D_800B0158 (*(s8 *)&D_800B0158_o)
extern struct { char _[16]; } D_800B0159_o __asm__("D_800B0159");
#define D_800B0159 (*(s8 *)&D_800B0159_o)
extern struct { char _[16]; } D_800B015A_o __asm__("D_800B015A");
#define D_800B015A (*(s8 *)&D_800B015A_o)
extern struct { char _[16]; } D_800B0160_o __asm__("D_800B0160");
#define D_800B0160 (*(s8 *)&D_800B0160_o)
extern struct { char _[16]; } D_800B0161_o __asm__("D_800B0161");
#define D_800B0161 (*(s8 *)&D_800B0161_o)
extern struct { char _[16]; } D_800B0162_o __asm__("D_800B0162");
#define D_800B0162 (*(s8 *)&D_800B0162_o)
extern struct { char _[16]; } D_800B0168_o __asm__("D_800B0168");
#define D_800B0168 (*(s8 *)&D_800B0168_o)
extern struct { char _[16]; } D_800B0169_o __asm__("D_800B0169");
#define D_800B0169 (*(s8 *)&D_800B0169_o)
extern struct { char _[16]; } D_800B016A_o __asm__("D_800B016A");
#define D_800B016A (*(s8 *)&D_800B016A_o)
extern struct { char _[16]; } D_800B0170_o __asm__("D_800B0170");
#define D_800B0170 (*(s8 *)&D_800B0170_o)
extern struct { char _[16]; } D_800B0171_o __asm__("D_800B0171");
#define D_800B0171 (*(s8 *)&D_800B0171_o)
extern struct { char _[16]; } D_800B0172_o __asm__("D_800B0172");
#define D_800B0172 (*(s8 *)&D_800B0172_o)
extern struct { char _[16]; } D_800B017C_o __asm__("D_800B017C");
#define D_800B017C (*(s8 *)&D_800B017C_o)
extern struct { char _[16]; } D_800B017D_o __asm__("D_800B017D");
#define D_800B017D (*(s8 *)&D_800B017D_o)
extern struct { char _[16]; } D_800B017E_o __asm__("D_800B017E");
#define D_800B017E (*(s8 *)&D_800B017E_o)
extern struct { char _[16]; } D_800B0184_o __asm__("D_800B0184");
#define D_800B0184 (*(s8 *)&D_800B0184_o)
extern struct { char _[16]; } D_800B0185_o __asm__("D_800B0185");
#define D_800B0185 (*(s8 *)&D_800B0185_o)
extern struct { char _[16]; } D_800B0186_o __asm__("D_800B0186");
#define D_800B0186 (*(s8 *)&D_800B0186_o)
extern struct { char _[16]; } D_800B018C_o __asm__("D_800B018C");
#define D_800B018C (*(s8 *)&D_800B018C_o)
extern struct { char _[16]; } D_800B018D_o __asm__("D_800B018D");
#define D_800B018D (*(s8 *)&D_800B018D_o)
extern struct { char _[16]; } D_800B018E_o __asm__("D_800B018E");
#define D_800B018E (*(s8 *)&D_800B018E_o)
extern struct { char _[16]; } D_800B0194_o __asm__("D_800B0194");
#define D_800B0194 (*(s8 *)&D_800B0194_o)
extern struct { char _[16]; } D_800B0195_o __asm__("D_800B0195");
#define D_800B0195 (*(s8 *)&D_800B0195_o)
extern struct { char _[16]; } D_800B0196_o __asm__("D_800B0196");
#define D_800B0196 (*(s8 *)&D_800B0196_o)
extern struct { char _[16]; } D_800B01A0_o __asm__("D_800B01A0");
#define D_800B01A0 (*(s8 *)&D_800B01A0_o)
extern struct { char _[16]; } D_800B01A1_o __asm__("D_800B01A1");
#define D_800B01A1 (*(s8 *)&D_800B01A1_o)
extern struct { char _[16]; } D_800B01A2_o __asm__("D_800B01A2");
#define D_800B01A2 (*(s8 *)&D_800B01A2_o)
extern struct { char _[16]; } D_800B01A8_o __asm__("D_800B01A8");
#define D_800B01A8 (*(s8 *)&D_800B01A8_o)
extern struct { char _[16]; } D_800B01A9_o __asm__("D_800B01A9");
#define D_800B01A9 (*(s8 *)&D_800B01A9_o)
extern struct { char _[16]; } D_800B01AA_o __asm__("D_800B01AA");
#define D_800B01AA (*(s8 *)&D_800B01AA_o)
extern struct { char _[16]; } D_800B01B0_o __asm__("D_800B01B0");
#define D_800B01B0 (*(s8 *)&D_800B01B0_o)
extern struct { char _[16]; } D_800B01B1_o __asm__("D_800B01B1");
#define D_800B01B1 (*(s8 *)&D_800B01B1_o)
extern struct { char _[16]; } D_800B01B2_o __asm__("D_800B01B2");
#define D_800B01B2 (*(s8 *)&D_800B01B2_o)
extern struct { char _[16]; } D_800B01B8_o __asm__("D_800B01B8");
#define D_800B01B8 (*(s8 *)&D_800B01B8_o)
extern struct { char _[16]; } D_800B01B9_o __asm__("D_800B01B9");
#define D_800B01B9 (*(s8 *)&D_800B01B9_o)
extern struct { char _[16]; } D_800B01BA_o __asm__("D_800B01BA");
#define D_800B01BA (*(s8 *)&D_800B01BA_o)
extern s32 D_800B0CD8_r[] __asm__("g_GameState");
extern s32 D_800B0CD8_w[] __asm__("g_GameState");
extern struct { char _[16]; } D_800B692C_o __asm__("D_800B692C");
#define D_800B692C (*(s8 *)&D_800B692C_o)
extern struct { char _[16]; } D_800B692D_o __asm__("D_800B692D");
#define D_800B692D (*(s8 *)&D_800B692D_o)
extern struct { char _[16]; } D_800B692E_o __asm__("D_800B692E");
#define D_800B692E (*(s8 *)&D_800B692E_o)
extern struct { char _[16]; } D_800B6948_o __asm__("D_800B6948");
#define D_800B6948 (*(s8 *)&D_800B6948_o)
extern struct { char _[16]; } D_800B6949_o __asm__("D_800B6949");
#define D_800B6949 (*(s8 *)&D_800B6949_o)
extern struct { char _[16]; } D_800B694A_o __asm__("D_800B694A");
#define D_800B694A (*(s8 *)&D_800B694A_o)

void Battle_StartDeathAnim(void) {
    s32 c2;
    s32 c3;
    s32 c4;
    s32 c5;
    s32 c6;
    register s32 c7 asm("$7");
    u8 *p254;
    s32 t98;

    Battle_ResetEnemyStats(0);
    c6 = 0x46;
    c5 = 0x82;
    c4 = 0x9F;
    c2 = 0xFF;
    c3 = 0xF9;
    D_800B00ED = c6;
    D_800B00FD = c6;
    D_800B0111 = c6;
    D_800B0121 = c6;
    c6 = 0x36;
    D_800B00F4 = c4;
    D_800B0104 = c4;
    D_800B692C = c4;
    D_800B0118 = c4;
    D_800B0128 = c4;
    D_800B6948 = c4;
    c4 = 0x4A;
    D_800B00F6 = c3;
    D_800B0106 = c3;
    D_800B692E = c3;
    D_800B011A = c3;
    D_800B012A = c3;
    D_800B694A = c3;
    c3 = 0x3B;
    c7 = 0x3D;
    D_800B00EC = 0;
    D_800B00EE = c5;
    D_800B00F5 = c2;
    D_800B00FC = 0;
    D_800B00FE = c5;
    D_800B0105 = c2;
    D_800B692D = c2;
    D_800B0110 = 0;
    D_800B0112 = c5;
    D_800B0119 = c2;
    D_800B0120 = 0;
    D_800B0122 = c5;
    D_800B0129 = c2;
    D_800B6949 = c2;
    D_800B0134 = 0;
    D_800B0135 = c5;
    D_800B0136 = c6;
    D_800B013C = c4;
    D_800B0146 = c6;
    D_800B017E = c6;
    D_800B018E = c6;
    c6 = 0x81;
    D_800B0145 = c5;
    D_800B017D = c5;
    D_800B018D = c5;
    c5 = 0x83;
    D_800B014C = c4;
    D_800B0184 = c4;
    D_800B0194 = c4;
    c4 = 0x13;
    D_800B013E = c3;
    D_800B014E = c3;
    D_800B0186 = c3;
    D_800B0196 = c3;
    c3 = 1;
    D_800B013D = c2;
    D_800B0144 = 0;
    D_800B014D = c2;
    D_800B017C = 0;
    D_800B0185 = c2;
    D_800B018C = 0;
    D_800B0195 = c2;
    D_800B0158 = c2;
    D_800B0159 = c7;
    D_800B015A = c6;
    D_800B0160 = c5;
    D_800B0161 = c4;
    D_800B0162 = c3;
    D_800B0168 = c2;
    D_800B0169 = c7;
    D_800B016A = c6;
    D_800B0170 = c5;
    D_800B0171 = c4;
    D_800B0172 = c3;
    D_800B01A0 = c2;
    D_800B01A1 = c7;
    D_800B01B0 = c2;
    c2 = 2;
    g_BattleModeState = c2;
    __asm__ __volatile__("");
    {
        s32 d2e8;
        d2e8 = g_FieldMoveLock_r[0];
        __asm__("" : "=r"(d2e8) : "0"(d2e8));
        D_800B01A8 = c5;
        D_800B01B8 = c5;
        p254 = g_PlayerEntity;
        __asm__ __volatile__("" :: "r"(p254));
        D_800B01AA = c3;
        D_800B01BA = c3;
        c3 = ~0x100;
        __asm__ __volatile__("" :: "r"(c3));
        D_800B01A2 = c6;
        D_800B01A9 = c4;
        D_800B01B1 = c7;
        D_800B01B2 = c6;
        D_800B01B9 = c4;
        __asm__ __volatile__("" : "=r"(d2e8) : "0"(d2e8));
        g_FieldMoveLock_w[0] = d2e8 | 1;
    }
    t98 = *(s32 *) (p254 + 0x98);
    *(s32 *) (p254 + 0x68) = 0;
    *(s32 *) (p254 + 0x6C) = 0;
    *(s32 *) (p254 + 0x70) = 0;
    *(s32 *) (p254 + 0x98) = t98 & c3;
    D_800B0CD8_r[0] = D_800B0CD8_r[0] | 0x8000;
    if (!(g_GameStateFlags & 0x1800)) {
        Entity_SetActionMode(p254, 0x14);
        {
            u8 *q;
            q = D254_B;
            func_8006DE80(0x45B, 0, *(s16 *) (q + 0x2A), *(s16 *) (q + 0x2E), (s32) *(s16 *) (q + 0x32));
        }
    }
}
