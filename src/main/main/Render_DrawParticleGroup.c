/* MASPSX_FLAGS: --stack-return-delay */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Render_DrawParticleSprite();
M2C_UNK Render_DrawParticleAlt();
M2C_UNK CdRom_CmdEventCallback();
M2C_UNK CdRom_ProcessEventByte();
extern M2C_UNK g_CdRomCmdTimeout[];
#define g_CdRomCmdTimeout (g_CdRomCmdTimeout[0])

void Render_DrawParticleGroup(s32 arg0, M2C_UNK arg1) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = arg0;
    CdRom_ProcessEventByte(var_s0 & 0xFF);

    {
        void *base = &g_CdRomCmdTimeout;
        s32 flags;

        __asm__ volatile("" : "=r"(base) : "0"(base));
        flags = M2C_FIELD(base, u8 *, -0x2C);
        __asm__ volatile("sw $0,4($3)" ::: "memory");
        M2C_FIELD(base, s32 *, 0) = 0;
        if (flags & 0x10) {
            var_s0 = 5;
        }
        temp_v1 = M2C_FIELD(base, s32 *, -0x28);
    }

    if (temp_v1 == 0x1F) {
        goto case_1f;
    }
    if (temp_v1 == 0x20) {
        goto case_20;
    }
    goto case_default;

case_1f:
    Render_DrawParticleSprite(var_s0 & 0xFF, arg1);
    goto after_dispatch;

case_20:
    Render_DrawParticleAlt(var_s0 & 0xFF, arg1);
    goto after_dispatch;

case_default:
    CdRom_CmdEventCallback(var_s0 & 0xFF, arg1);

after_dispatch:
    {
        void *base = &g_CdRomCmdTimeout;

        __asm__ volatile("" : "=r"(base) : "0"(base));
        if (M2C_FIELD(base, s32 *, 0) == 0) {
            M2C_FIELD(base, s32 *, -0x28) = 0x21;
        }
    }
}
