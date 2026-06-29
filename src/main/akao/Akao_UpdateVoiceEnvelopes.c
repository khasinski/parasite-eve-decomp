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

M2C_UNK SpuGetVoiceEnvelope();
M2C_UNK Akao_RemoveVoice();

extern struct { char _[16]; } g_AkaoCurTrack_o __asm__("g_AkaoCurTrack");
#define g_AkaoCurTrack (*(void **)&g_AkaoCurTrack_o)
extern struct { char _[16]; } D_800B002C_o __asm__("g_AkaoVoiceEnvelopeTable");
#define g_AkaoVoiceEnvelopeTable (*(s16 *)&D_800B002C_o)
extern struct { char _[16]; } D_800B8AC0_o __asm__("g_AkaoVoiceStateTable");
#define g_AkaoVoiceStateTable (*(M2C_UNK *)&D_800B8AC0_o)

asm(".globl Akao_UpdateVoiceEnvelopes");
asm("Akao_UpdateVoiceEnvelopes = func_80089250");

void Akao_UpdateVoiceEnvelopes(s32 arg0) __asm__("func_80089250");

void Akao_UpdateVoiceEnvelopes(s32 arg0) {
    u8 *var_s1;
    s32 temp_s3;
    u32 var_s0;
    u32 mask;
    u8 *voice_base;

    var_s0 = 0;
    mask = 1;
    voice_base = (u8 *) &g_AkaoVoiceStateTable;
    var_s1 = (u8 *) &g_AkaoVoiceEnvelopeTable;
    temp_s3 = (M2C_FIELD(g_AkaoCurTrack, s32 *, 4) & M2C_FIELD(g_AkaoCurTrack, s32 *, 0xC)) | (M2C_FIELD(g_AkaoCurTrack, s32 *, 0x6C) & M2C_FIELD(g_AkaoCurTrack, s32 *, 0x74)) | arg0;
    do {
        if (temp_s3 & (mask << var_s0)) {
            register s32 temp_v0 asm("$2");
            temp_v0 = 0x7FFF;
            M2C_FIELD(var_s1, s16 *, 0) = temp_v0;
        } else {
            SpuGetVoiceEnvelope(var_s0, var_s1);
            if (M2C_FIELD(var_s1, s16 *, 0) == 0) {
                Akao_RemoveVoice(voice_base, var_s0);
                Akao_RemoveVoice(voice_base + 0x1AA0, var_s0);
            }
        }
        var_s0 += 1;
        var_s1 += 8;
    } while (var_s0 < 0x18U);
}
