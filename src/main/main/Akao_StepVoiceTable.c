/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Akao_Cmd_98_9A_9C();
M2C_UNK Akao_Cmd_99_9B_9D();
M2C_UNK DrawPrim();
M2C_UNK PutDrawEnv();
extern s32 D_8009CDDC_a1[] __asm__("g_ActiveDrawSlot");
extern s32 D_8009CDDC_a2[] __asm__("g_ActiveDrawSlot");
extern s32 D_8009CDDC_a3[] __asm__("g_ActiveDrawSlot");
extern s32 D_8009CDDC_a4[] __asm__("g_ActiveDrawSlot");
extern s32 D_8009CDDC_a5[] __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
#define g_GameState (*(s32 *)&D_800B0CD8_o)
extern struct { char _[16]; } D_800BCDC8_o __asm__("g_RenderDrawEnvArray");
extern struct { char _[16]; } D_800BCDDF_o __asm__("g_RenderDrawEnvIsbg");
extern struct { char _[16]; } D_800BCDDF_o2 __asm__("g_RenderDrawEnvIsbg");
extern struct { char _[16]; } D_800BCDE0_o __asm__("D_800BCDE0");
extern struct { char _[16]; } D_800BCDE0_o2 __asm__("D_800BCDE0");
#define D_800BCDDF_AT(off) (*((u8 *)&D_800BCDDF_o + (off)))
#define D_800BCDDF_AT2(off) (*((u8 *)&D_800BCDDF_o2 + (off)))
#define D_800BCDE0_AT(off) (*((u8 *)&D_800BCDE0_o + (off)))
#define D_800BCDE0_AT2(off) (*((u8 *)&D_800BCDE0_o2 + (off)))

void Akao_StepVoiceTable(void) {
    s32 one;

    if (!(g_GameState & 0x200) && (g_GameStateFlags & 0x10)) {
        one = 1;
        D_800BCDE0_AT(D_8009CDDC_a1[0] * 0x5C) = 0;
        D_800BCDDF_AT(D_8009CDDC_a2[0] * 0x5C) = one;
        PutDrawEnv((u8 *)&D_800BCDC8_o + (D_8009CDDC_a3[0] * 0x5C));
        DrawPrim((u8 *)&D_800B0CD8_o + 0x114);
        DrawPrim((u8 *)&D_800B0CD8_o + 0x104);
        D_800BCDE0_AT2(D_8009CDDC_a4[0] * 0x5C) = one;
        D_800BCDDF_AT2(D_8009CDDC_a5[0] * 0x5C) = 0;
        Akao_Cmd_99_9B_9D(0);
    }
    if (g_GameStateFlags & 0x20) {
        Akao_Cmd_98_9A_9C(0);
    }
}
