/* CC1_FLAGS: -g3 -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "pe1/akao.h"

void SeqOp_SetVoiceInstrument(AkaoTrack *track, AkaoInstrument *instrument, int sample_header);
extern struct { char _[16]; } g_AkaoCurTrack_o __asm__("g_AkaoCurTrack");
#define g_AkaoCurTrack (*(s32 **)&g_AkaoCurTrack_o)
extern AkaoInstrument g_AkaoInstrumentTable[];

void Akao_SetVoiceLoopAddrAlt(void *arg0, u32 arg1) {
    s32 temp_a1_2;
    s32 temp_a2;
    s32 var_a0;
    s32 var_v1;
    s32 temp_a1;
    s32 temp_a0_2;
    u16 var_a1;
    u8 temp_a0;
    u8 temp_v0;
    s32 temp_v1;
    u8 tmp6;
    AkaoVoiceLoopRange *var_s0;

    var_a0 = 1;
    var_s0 = (AkaoVoiceLoopRange *)((AkaoTrack *)arg0)->repeat_target;
    temp_a2 = -((*g_AkaoCurTrack & 0x100) != 0) & 0x30;
    while (var_s0->note < 0x80U) {
        var_s0 += 1;
        var_a0 += 1;
    }
    var_s0 = (AkaoVoiceLoopRange *)((AkaoTrack *)arg0)->repeat_target;
    var_v1 = var_a0;
    __asm__("" : "=r"(var_v1) : "0"(var_v1));
    var_s0 = (AkaoVoiceLoopRange *)((u8 *)var_s0 + ((var_v1 - 1) * 8));
    if (var_v1 != 0) {
loop_4:
        if (arg1 < var_s0->min_value) {
            var_v1 -= 1;
            var_s0 -= 1;
            if (var_v1 != 0) {
                goto loop_4;
            }
        }
    }
    temp_a0 = var_s0->note;
    temp_a1 = ((AkaoTrack *)arg0)->note_pitch;
    if (temp_a0 < 0x20U) {
        if (temp_a1 != temp_a0) {
            goto block_10;
        }
    } else if (temp_a1 != (temp_a0 + temp_a2)) {
block_10:
        if (var_v1 != 0) {
            temp_v1 = var_s0[-1].note;
            temp_a0_2 = ((AkaoTrack *)arg0)->note_pitch;
            if (temp_v1 < 0x20U) {
                if (temp_a0_2 != temp_v1) {
                    goto block_15;
                }
            } else if (temp_a0_2 != (temp_v1 + temp_a2)) {
                goto block_15;
            }
        } else {
block_15:
            temp_v0 = var_s0->note;
            var_a1 = temp_v0 & 0xFF;
            if (temp_v0 >= 0x20U) {
                var_a1 += temp_a2;
            }
            ((AkaoTrack *)arg0)->note_pitch = var_a1;
            temp_a1_2 = var_a1 << 6;
            SeqOp_SetVoiceInstrument(arg0, (AkaoInstrument *)((u8 *)&g_AkaoInstrumentTable + temp_a1_2), *(s32 *)((u8 *)&g_AkaoInstrumentTable + temp_a1_2));
            ((AkaoTrack *)arg0)->adsr_attack_rate = var_s0->adsr_attack_rate;
            ((AkaoTrack *)arg0)->adsr_sustain_rate = var_s0->adsr_sustain_rate;
            ((AkaoTrack *)arg0)->adsr_sustain = var_s0->adsr_sustain_mode;
            tmp6 = var_s0->adsr_release_rate;
            ((AkaoTrack *)arg0)->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE_RATE;
            ((AkaoTrack *)arg0)->adsr_release_rate = tmp6;
        }
    }
}
