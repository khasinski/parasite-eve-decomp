/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK AkaoSpuVoice_SetAdsrAttack();
M2C_UNK AkaoSpuVoice_SetAdsrDecayRate();
M2C_UNK AkaoSpuVoice_SetAdsrReleaseRate();
M2C_UNK AkaoSpuVoice_SetAdsrSustainLevel();
M2C_UNK AkaoSpuVoice_SetAdsrSustainRate();
M2C_UNK AkaoSpuVoice_SetPitch(s32, u16);
M2C_UNK AkaoSpuVoice_SetRepeatAddress();
M2C_UNK AkaoSpuVoice_SetStartAddress();
M2C_UNK AkaoSpuVoice_SetVolume();

void Akao_WriteVoiceParam(s32 arg0, void *arg1) {
    s32 flags;
    s32 cur;

    flags = M2C_FIELD(arg1, s32 *, 4);
    if (flags != 0) {
        if (flags & 0x10) {
            AkaoSpuVoice_SetPitch(arg0, M2C_FIELD(arg1, u16 *, 0x1C));
            {
                register s32 mask asm("$3") = ~0x10;
                cur = M2C_FIELD(arg1, s32 *, 4) & mask;
            }
            M2C_FIELD(arg1, s32 *, 4) = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & 3) {
            AkaoSpuVoice_SetVolume(arg0, M2C_FIELD(arg1, s16 *, 0x28), M2C_FIELD(arg1, s16 *, 0x2A));
            {
                register s32 mask asm("$3") = ~3;
                cur = M2C_FIELD(arg1, s32 *, 4) & mask;
            }
            M2C_FIELD(arg1, s32 *, 4) = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & 0x80) {
            AkaoSpuVoice_SetStartAddress(arg0, M2C_FIELD(arg1, s32 *, 8));
            {
                register s32 mask asm("$3") = ~0x80;
                cur = M2C_FIELD(arg1, s32 *, 4) & mask;
            }
            M2C_FIELD(arg1, s32 *, 4) = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & 0x10000) {
            AkaoSpuVoice_SetRepeatAddress(arg0, M2C_FIELD(arg1, s32 *, 0xC));
            {
                register s32 mask asm("$3") = 0xFFFEFFFF;
                cur = M2C_FIELD(arg1, s32 *, 4) & mask;
            }
            M2C_FIELD(arg1, s32 *, 4) = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & 0x2200) {
            AkaoSpuVoice_SetAdsrSustainRate(arg0, M2C_FIELD(arg1, u16 *, 0x24), M2C_FIELD(arg1, s32 *, 0x14));
            {
                register s32 mask asm("$3") = ~0x2200;
                cur = M2C_FIELD(arg1, s32 *, 4) & mask;
            }
            M2C_FIELD(arg1, s32 *, 4) = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & 0x900) {
            AkaoSpuVoice_SetAdsrAttack(arg0, M2C_FIELD(arg1, u16 *, 0x1E), M2C_FIELD(arg1, s32 *, 0x10));
            {
                register s32 mask asm("$3") = ~0x900;
                cur = M2C_FIELD(arg1, s32 *, 4) & mask;
            }
            M2C_FIELD(arg1, s32 *, 4) = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & 0x4400) {
            AkaoSpuVoice_SetAdsrReleaseRate(arg0, M2C_FIELD(arg1, u16 *, 0x26), M2C_FIELD(arg1, s32 *, 0x18));
            {
                register s32 mask asm("$3") = ~0x4400;
                cur = M2C_FIELD(arg1, s32 *, 4) & mask;
            }
            M2C_FIELD(arg1, s32 *, 4) = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & 0x9000) {
            AkaoSpuVoice_SetAdsrDecayRate(arg0, M2C_FIELD(arg1, u16 *, 0x20));
            AkaoSpuVoice_SetAdsrSustainLevel(arg0, M2C_FIELD(arg1, u16 *, 0x22));
        }
        M2C_FIELD(arg1, s32 *, 4) = 0;
    }
}
