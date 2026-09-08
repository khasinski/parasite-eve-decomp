#include "common.h"
#define NULL ((void *)0)
#include "pe1/akao.h"

void SeqOp_SetVoiceInstrument(AkaoTrack *track, AkaoInstrument *instrument, int sample_header);
extern s32 *g_AkaoCurTrack;
extern AkaoInstrument g_AkaoInstrumentTable[];

void Akao_SetVoiceLoopAddr(AkaoTrack *track, u32 arg1) {
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a0;
    s32 temp_a0_2;
    u16 var_a1;
    u32 var_a0;
    u32 var_v1;
    u8 temp_v0;
    u8 tmp6;
    s32 temp_v1;
    s32 temp_v1_2;
    AkaoVoiceLoopRange *var_s0;
    s32 pad_[2];

    var_a0 = 1;
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
    temp_a2 = -((*g_AkaoCurTrack & 0x100) != 0) & 0x30;
    while (var_s0[1].note < 0x80U) {
        var_s0 += 1;
        var_a0 += 1;
    }
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
    var_v1 = 0;
    if (var_a0 != 0) {
loop_4:
        var_v1 += 1;
        if (var_s0->max_value < arg1) {
            var_s0 += 1;
            if (var_v1 < var_a0) {
                goto loop_4;
            }
        }
    }
    temp_v1 = var_s0->note;
    temp_a0 = track->note_pitch;
    if (temp_v1 < 0x20U) {
        if (temp_a0 != temp_v1) {
            goto block_10;
        }
    } else if (temp_a0 != (temp_v1 + temp_a2)) {
block_10:
        temp_v1_2 = var_s0[1].note;
        temp_a0_2 = track->note_pitch;
        if (temp_v1_2 < 0x20U) {
            if (temp_a0_2 != temp_v1_2) {
                goto block_14;
            }
        } else if (temp_a0_2 != (temp_v1_2 + temp_a2)) {
block_14:
            temp_v0 = var_s0->note;
            var_a1 = temp_v0 & 0xFF;
            if (temp_v0 >= 0x20U) {
                var_a1 += temp_a2;
            }
            track->note_pitch = var_a1;
            temp_a1 = var_a1 << 6;
            SeqOp_SetVoiceInstrument(track, (AkaoInstrument *)((u8 *)&g_AkaoInstrumentTable + temp_a1), *(s32 *)((u8 *)&g_AkaoInstrumentTable + temp_a1));
            track->adsr_attack_rate = var_s0->adsr_attack_rate;
            track->adsr_sustain_rate = var_s0->adsr_sustain_rate;
            track->adsr_sustain = var_s0->adsr_sustain_mode;
            tmp6 = var_s0->adsr_release_rate;
            track->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE_RATE;
            track->adsr_release_rate = tmp6;
        }
    }
}

void Akao_SetVoiceLoopAddrAlt(AkaoTrack *track, u32 arg1) {
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
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
    temp_a2 = -((*g_AkaoCurTrack & 0x100) != 0) & 0x30;
    while (var_s0->note < 0x80U) {
        var_s0 += 1;
        var_a0 += 1;
    }
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
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
    temp_a1 = track->note_pitch;
    if (temp_a0 < 0x20U) {
        if (temp_a1 != temp_a0) {
            goto block_10;
        }
    } else if (temp_a1 != (temp_a0 + temp_a2)) {
block_10:
        if (var_v1 != 0) {
            temp_v1 = var_s0[-1].note;
            temp_a0_2 = track->note_pitch;
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
            track->note_pitch = var_a1;
            temp_a1_2 = var_a1 << 6;
            SeqOp_SetVoiceInstrument(track, (AkaoInstrument *)((u8 *)&g_AkaoInstrumentTable + temp_a1_2), *(s32 *)((u8 *)&g_AkaoInstrumentTable + temp_a1_2));
            track->adsr_attack_rate = var_s0->adsr_attack_rate;
            track->adsr_sustain_rate = var_s0->adsr_sustain_rate;
            track->adsr_sustain = var_s0->adsr_sustain_mode;
            tmp6 = var_s0->adsr_release_rate;
            track->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE_RATE;
            track->adsr_release_rate = tmp6;
        }
    }
}

extern int g_AkaoPitchPeriodTable[];

void Akao_SetNotePitchBounded(AkaoTrack *track, int arg1) {
    int value = track->field_E2;

    if ((unsigned int)value < (unsigned int)arg1) {
        Akao_SetVoiceLoopAddr(track, arg1);
    } else if ((unsigned int)arg1 < (unsigned int)value) {
        Akao_SetVoiceLoopAddrAlt(track, arg1);
    }
}

int Akao_LookupPitchPeriod(int arg0, int arg1, int arg2) {
    register int offset asm("$2");
    int row;
    register int shift asm("$5");
    int shamt;
    int value;
    int table;
    int *base;
    int row_offset;

    arg1 = (u8)arg1;
    row_offset = arg0 << 6;
    table = (int)g_AkaoPitchPeriodTable;
    base = (int *)(row_offset + table);
    row = (unsigned int)arg1 / 12;
    offset = arg1 - (row * 12);
    offset <<= 2;
    value = *(int *)(offset + (int)base);
    shift = row;

    if (arg2 != 0) {
        value += (unsigned int)(value * arg2) >> 7;
    }

    if ((unsigned int)shift < 7) {
        goto less_than_7;
    }
    shamt = shift - 6;
    value <<= shamt;
    goto done;

less_than_7:
    if ((unsigned int)row >= 6) {
        goto done;
    }
    shamt = 6 - row;
    value = (unsigned int)value >> shamt;

done:
    return value & 0xFFFF;
}
