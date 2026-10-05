#include "common.h"
#include "pe1/akao/spu_common.h"
#include "pe1/psyq_spu_internal.h"
#include "pe1/akao/track.h"


extern unsigned short D_8009D2B6;
extern SpuCommonSettings D_800C0D90;
/* These field aliases keep the retail absolute stores byte-identical. */
extern unsigned short D_800C0DA0;
extern unsigned short D_800C0DA2;
extern int D_800C0DA4;

void Seq_GetGlobalPitch(unsigned int *out)
{
    *out = D_8009B3A0.mode;
}

void Seq_ApplyGlobalPitch(void) {
    unsigned short value = D_8009D2B6;
    char *regs;

    __asm__("" : "=r"(regs) : "0"(&D_800C0D90));
    *(int *)regs = 0x1C0;
    D_800C0DA4 = 0;
    D_800C0DA2 = value;
    D_800C0DA0 = value;
    SpuSetCommonAttr((SpuCommonSettings *)regs);
}

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size)
{
    size >>= 2;
    do {
        *dst = *src;
        src++;
        size--;
        dst++;
    } while (size != 0);
}
extern u16 D_8009CDEC;
extern s16 D_8009D2A2, D_8009D220, D_8009D21E;
extern u32 D_8009D2B4, D_8009D284, D_8009D2D0, D_8009D214;
extern u32 D_8009D2CC, D_8009D210, D_800BCD50;
/* These aliases share storage with AkaoTickBank/AkaoTickTrack in tick.h. */
extern AkaoSequencerBank *volatile D_8009D2C8_Spu asm("D_8009D2C8");
extern AkaoTrack D_800B8AC0_Spu[] asm("D_800B8AC0");
extern AkaoTrack D_800BA560[];
extern AkaoTrack D_800BC000_Spu[] asm("D_800BC000");
void Seq_ApplyGlobalPitch(void);
void func_8008AB9C(AkaoTrack *);

/* Advance global, bank and voice slides every fourth tick.
 * Matching debt: old is pinned to v1; the empty memory barrier preserves
 * the cursor reload after the callback-dependent bank store. */
void SPU_StepReverbLoad(void)
{
    u32 value;
    register u32 old asm("$3");
    u32 pending;
    int count;
    AkaoTrack *track;
    AkaoSequencerBank *bank;
    if ((++D_8009CDEC & 3) != 0)
        return;
    if (D_8009D2A2 != 0) {
        D_8009D2A2--;
        D_8009D2B4 += D_8009D284;
        Seq_ApplyGlobalPitch();
    }
    if (D_8009D220 != 0) {
        D_8009D220--;
        D_8009D2D0 += D_8009D214;
    }
    if (D_8009D21E != 0) {
        old = D_8009D2CC;
        D_8009D21E--;
        value = old + D_8009D210;
        if ((value & 0xFF0000) != (old & 0xFF0000)) {
            track = D_800B8AC0_Spu;
            for (count = 24; count != 0; --count, ++track)
                track->update_flags |= 0x10;
        }
        D_8009D2CC = value;
    }
    bank = D_8009D2C8_Spu;
    if (bank->active_voice_mask && bank->pitch_slide_duration) {
        old = bank->pitch_current;
        bank->pitch_slide_duration--;
        value = old + bank->pitch_delta;
        if ((value & 0x7F0000) != (old & 0x7F0000))
            func_8008AB9C(D_800B8AC0_Spu);
        D_8009D2C8_Spu->pitch_current = value;
        asm volatile("" ::: "memory");
        bank = D_8009D2C8_Spu;
    }
    D_8009D2C8_Spu = bank + 1;
    if (bank[1].active_voice_mask && bank[1].pitch_slide_duration) {
        old = bank[1].pitch_current;
        bank[1].pitch_slide_duration--;
        value = old + bank[1].pitch_delta;
        if ((value & 0x7F0000) != (old & 0x7F0000))
            func_8008AB9C(D_800BA560);
        D_8009D2C8_Spu->pitch_current = value;
    }
    D_8009D2C8_Spu = D_8009D2C8_Spu - 1;
    pending = D_800BCD50;
    if (pending) {
        count = 0x1000;
        track = D_800BC000_Spu;
        do {
            if (pending & count) {
                if (track->panpot_duration) {
                    old = (s16)track->pan_target;
                    track->panpot_duration--;
                    value = old + (s16)track->pan_delta;
                    if ((value & 0xFF00) != (old & 0xFF00))
                        track->update_flags |= 3;
                    track->pan_target = value;
                }
                if (track->panpot_slide_duration) {
                    old = track->panpot;
                    track->panpot_slide_duration--;
                    value = old + (s16)track->panpot_delta;
                    if ((value & 0xFF00) != (old & 0xFF00))
                        track->update_flags |= 3;
                    track->panpot = value;
                }
                if (track->field_70) {
                    old = track->voice_mask_b;
                    track->field_70--;
                    value = old + track->pan_base;
                    if ((value & 0xFF00) != (old & 0xFF00))
                        track->update_flags |= 0x10;
                    track->voice_mask_b = value;
                }
                pending ^= count;
            }
            count = count << 1;
            ++track;
        } while (pending);
    }
}

#include "common.h"
#include "pe1/akao/tick.h"

/* AKAO timer tick: advances the primary and secondary sequencer banks by
 * their scaled tempo, steps every active track, and runs the sound-effect
 * voices, global slides and message queue. */
void Akao_Tick(void)
{
    AkaoTickTrack *track;
    u32 mask;
    u32 bit;
    u32 tempo;
    u32 accumulator;
    u32 scale;

    Akao_ProcessVoiceQueue();
    if (D_8009D2C8->timing.active_voice_mask) {
        scale = D_8009D2D2;
        tempo = D_8009D2C8->timing.tempo.half.integer;
        if (scale) {
            if (scale < 0x80)
                tempo += (tempo * scale) >> 7;
            else
                tempo = (tempo * scale) >> 8;
        }
        D_8009D2C8->timing.tick_accumulator += tempo;
        if ((D_8009D2C8->timing.tick_accumulator & 0xFFFF0000) || (D_8009D2DC & 4)) {
            D_8009D2C8->timing.tick_accumulator &= 0xFFFF;
            do {
                bit = 1;
                track = D_800B8AC0;
                mask = D_8009D2C8->timing.active_voice_mask;
                do {
                    if (mask & bit) {
                        track->timing.note_length--;
                        track->timing.gate_length--;
                        if (track->timing.note_length == 0) {
                            Akao_StepSampleLoader(&track->track, bit);
                        } else if (track->timing.gate_length == 0) {
                            D_8009D2C8->timing.key_off_request_mask |= bit;
                            D_8009D2C8->timing.allocated_voice_mask &= ~bit;
                        }
                        Spu_UpdateVoiceRegisters(&track->track, bit);
                        mask ^= bit;
                    }
                    track++;
                    bit <<= 1;
                } while (mask);
                if (D_8009D2C8->timing.tempo_slide_duration) {
                    D_8009D2C8->timing.tempo_slide_duration--;
                    D_8009D2C8->timing.tempo.value += D_8009D2C8->timing.tempo_delta;
                }
                if (D_8009D2C8->timing.volume_slide_duration) {
                    /* Updated through scalar pointers: retail keeps the
                     * update-flag read below both stores. */
                    AkaoU16 *duration = &D_8009D2C8->timing.volume_slide_duration;
                    AkaoU32 *volume = &D_8009D2C8->timing.volume;

                    (*duration)--;
                    *volume += D_8009D2C8->timing.volume_delta;
                    D_8009D2C4 |= 0x80;
                }
                if (D_8009D2C8->timing.ticks_per_beat) {
                    if (++D_8009D2C8->timing.tick == D_8009D2C8->timing.ticks_per_beat) {
                        D_8009D2C8->timing.tick = 0;
                        if (++D_8009D2C8->timing.beat == D_8009D2C8->timing.beats_per_measure) {
                            D_8009D2C8->timing.beat = 0;
                            D_8009D2C8->timing.measure++;
                            if (D_8009D22C)
                                D_8009D22C--;
                        }
                    }
                }
            } while (D_8009D22C);
        }
    }

    if (D_8009D2C8[1].timing.active_voice_mask) {
        D_8009D2C8 = D_8009D2C8 + 1;
        tempo = D_8009D2C8->timing.tempo.half.integer;
        scale = D_8009D2D2;
        if (scale) {
            if (scale < 0x80)
                tempo += (tempo * scale) >> 7;
            else
                tempo = (tempo * scale) >> 8;
        }
        D_8009D2C8->timing.tick_accumulator += tempo;
        if ((D_8009D2C8->timing.tick_accumulator & 0xFFFF0000) || (D_8009D2DC & 4)) {
            D_8009D2C8->timing.tick_accumulator &= 0xFFFF;
            bit = 1;
            track = &D_800B8AC0[24];
            mask = D_8009D2C8->timing.active_voice_mask;
            do {
                if (mask & bit) {
                    track->timing.note_length--;
                    track->timing.gate_length--;
                    if (track->timing.note_length == 0) {
                        Akao_StepSampleLoader(&track->track, bit);
                    } else if (track->timing.gate_length == 0) {
                        D_8009D2C8->timing.key_off_request_mask |= bit;
                        D_8009D2C8->timing.allocated_voice_mask &= ~bit;
                    }
                    Spu_UpdateVoiceRegisters(&track->track, bit);
                    mask ^= bit;
                }
                track++;
                bit <<= 1;
            } while (mask);
            if (D_8009D2C8->timing.tempo_slide_duration) {
                D_8009D2C8->timing.tempo_slide_duration--;
                D_8009D2C8->timing.tempo.value += D_8009D2C8->timing.tempo_delta;
            }
            if (D_8009D2C8->timing.volume_slide_duration) {
                D_8009D2C8->timing.volume_slide_duration--;
                D_8009D2C8->timing.volume += D_8009D2C8->timing.volume_delta;
            }
            if (D_8009D2C8->timing.ticks_per_beat) {
                if (++D_8009D2C8->timing.tick == D_8009D2C8->timing.ticks_per_beat) {
                    D_8009D2C8->timing.tick = 0;
                    if (++D_8009D2C8->timing.beat == D_8009D2C8->timing.beats_per_measure) {
                        D_8009D2C8->timing.beat = 0;
                        D_8009D2C8->timing.measure++;
                        if (D_8009D22C)
                            D_8009D22C--;
                    }
                }
            }
        }
        D_8009D2C8 = D_8009D2C8 - 1;
    }

    if (!D_8009D2C8->timing.active_voice_mask && !D_8009D2C8->timing.pending_restore_mask
        && D_8009D2C8[1].timing.active_voice_mask) {
        Util_CopyWords(D_8009D2C8[1].words, D_8009D2C8->words, sizeof(AkaoTickBank));
        Util_CopyWords(D_800B8AC0[24].words, D_800B8AC0[0].words, sizeof(AkaoTickTrack) * 24);
        D_8009D2C8[1].timing.bank_id = 0;
        D_8009D2C8[1].timing.active_voice_mask = 0;
    }

    mask = D_800BCD50;
    if (mask) {
        accumulator = D_800BCD68 + D_800BCD66;
        D_800BCD68 = accumulator;
        if ((accumulator & 0xFFFF0000) || (D_8009D2DC & 4)) {
            D_800BCD68 = accumulator & 0xFFFF;
            bit = 0x1000;
            track = D_800BC000;
            do {
                if (mask & bit) {
                    if (!(D_8009D2DC & 2) || (track->timing.key_on_mask & 0x2000000)) {
                        track->timing.note_length--;
                        track->timing.tick_count++;
                        track->timing.gate_length--;
                        if (track->timing.note_length == 0) {
                            Akao_StepSampleLoader(&track->track, bit);
                        } else if (track->timing.gate_length == 0) {
                            D_800BCD5C |= bit;
                            D_800BCD58 &= ~bit;
                        }
                        Spu_TickVoiceEnvelopes(&track->track, bit);
                    }
                    mask ^= bit;
                }
                track++;
                bit <<= 1;
            } while (mask);
        }
    }

    if (!D_8009D268)
        Akao_ProcessMessageQueue();
    SPU_StepReverbLoad();
    Akao_SetVoicePitch();
}

/* Timer callback immediately follows the sequencer tick and measures its
 * duration to maintain the rolling AKAO timer history. */
s32 GetRCnt(s32 arg0);

extern s32 D_8009B7EC;
extern s32 g_AkaoTimerDeltaHist2;
extern s32 g_AkaoTimerDeltaHist1;
extern s32 g_AkaoTimerDeltaHist0;
extern s32 D_8009CDE4;

void Akao_TimerCallback(void) {
    s32 delta;
    s32 old_f0;
    s32 old_f4;
    s32 old_f8;
    register s32 value asm("$2");

    delta = GetRCnt(0xF2000002);
    Akao_Tick();
    delta = GetRCnt(0xF2000002) - delta;
    if (delta <= 0) {
        delta += 0x44E8;
    }
    old_f0 = g_AkaoTimerDeltaHist2;
    old_f4 = g_AkaoTimerDeltaHist1;
    old_f8 = g_AkaoTimerDeltaHist0;
    value = delta;
    g_AkaoTimerDeltaHist0 = value;
    D_8009B7EC = old_f0;
    old_f0 = old_f0 + old_f4;
    old_f0 = old_f0 + old_f8;
    old_f0 = old_f0 + value;

    g_AkaoTimerDeltaHist2 = old_f4;
    g_AkaoTimerDeltaHist1 = old_f8;
    D_8009CDE4 = old_f0;
}


extern AkaoU8 D_8009B7BC[];
extern AkaoU8 D_8009B8BC[];

int Akao_LookupSampleBankByte(AkaoTrack *track)
{
    AkaoU8 *cursor = track->pc;
    register unsigned repeat __asm__("$6") = track->call_stack_index;
    unsigned code;
    int argument;
    int displacement;
    unsigned bank_byte;
    unsigned skip;

    for (;;) {
        code = *cursor;
check_code:
        if (code < 0x9A) {
            if (code >= 0x8F) {
                track->tremolo_duration = 0;
                track->tremolo_phase &= ~5;
            }
            return *cursor;
        }
        if (code < 0xA0) {
            return 0xA0;
        }
        skip = D_8009B7BC[code];
        if (skip) goto advance_by_table;
        switch (code) {
        case 0xFC:
            cursor++;
            argument = *cursor;
            skip = D_8009B8BC[argument];
            if (skip) goto advance_by_table;
            if (argument == 7) {
                goto check_bank;
            }
            if (argument < 8) {
                if (argument != 6) {
                    code = argument;
                    goto check_code;
                }
                goto handle_6;
            }
            if (argument < 10) {
                cursor++;
                if (*cursor == track->repeat_counters[repeat] + 1) {
                    cursor++;
                    argument = *cursor++;
                    displacement = *cursor++;
                    repeat = (repeat - 1) & 3;
                    goto apply_displacement;
                } else {
                    cursor += 3;
                }
                break;
            }
            break;
handle_6:
            cursor++;
            goto read_displacement;
check_bank:
            cursor++;
            bank_byte = *cursor;
            cursor++;
            if (D_8009D2C8->bank.field_56 < bank_byte) goto skip_two;
            goto read_displacement;
read_displacement:
            argument = *cursor++;
            displacement = *cursor++;
apply_displacement:
            argument += displacement << 8;
            displacement = (short)argument;
            cursor += displacement;
            break;
advance_by_table:
            cursor += skip;
            break;
skip_two:
            cursor += 2;
            break;
        case 0xC9:
            cursor++;
            if (*cursor == track->repeat_counters[repeat] + 1) {
                cursor++;
                repeat = (repeat - 1) & 3;
            } else {
                goto load_stack;
            }
            break;
        case 0xCB:
        case 0xCD:
        case 0xD1:
        case 0xDB:
            cursor++;
            track->tremolo_duration = 0;
            track->tremolo_phase &= ~5;
            break;
        case 0xCC:
        case 0xD0:
            track->tremolo_phase &= ~5;
            return 0xA0;
        case 0xCA:
            if (track->flags & AKAO_TRACK_FLAG_KEY_OFF_PENDING) {
                goto stop;
            }
load_stack:
            cursor = track->call_stack[repeat];
            break;
        default:
stop:
            track->tremolo_duration = 0;
            track->tremolo_phase &= ~5;
            return 0xA0;
        }
        continue;
    }
}
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
    s32 count_or_note;
    s32 var_v1;
    s32 temp_a1;
    s32 temp_a0_2;
    u16 var_a1;
    u8 temp_v0;
    s32 temp_v1;
    u8 tmp6;
    AkaoVoiceLoopRange *var_s0;

    count_or_note = 1;
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
    temp_a2 = -((*g_AkaoCurTrack & 0x100) != 0) & 0x30;
    while (var_s0->note < 0x80U) {
        var_s0 += 1;
        count_or_note += 1;
    }
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
    var_v1 = count_or_note;
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
    count_or_note = var_s0->note;
    temp_a1 = track->note_pitch;
    if (count_or_note < 0x20U) {
        if (temp_a1 != count_or_note) {
            goto block_10;
        }
    } else if (temp_a1 != (count_or_note + temp_a2)) {
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
    int value = track->current_note;

    if ((unsigned int)value < (unsigned int)arg1) {
        Akao_SetVoiceLoopAddr(track, arg1);
    } else if ((unsigned int)arg1 < (unsigned int)value) {
        Akao_SetVoiceLoopAddrAlt(track, arg1);
    }
}

int Akao_LookupPitchPeriod(int arg0, int arg1, int arg2) {
    int offset;
    int row;
    register int shift asm("$5");
    int address_or_period;
    int row_offset;

    arg1 = (u8)arg1;
    row_offset = arg0 << 6;
    address_or_period = (int)g_AkaoPitchPeriodTable;
    row = (unsigned int)arg1 / 12;
    offset = arg1 - (row * 12);
    offset <<= 2;
    row_offset += address_or_period;
    offset += row_offset;
    address_or_period = *(int *)offset;
    shift = row;

    if (arg2 != 0) {
        address_or_period += (unsigned int)(address_or_period * arg2) >> 7;
    }

    if ((unsigned int)shift < 7) {
        goto less_than_7;
    }
    offset = shift - 6;
    address_or_period <<= offset;
    goto done;

less_than_7:
    if ((unsigned int)row >= 6) {
        goto done;
    }
    offset = 6 - row;
    address_or_period = (unsigned int)address_or_period >> offset;

done:
    return address_or_period & 0xFFFF;
}
/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao/note_step.h"

/* Switches the track to a drum kit sample's instrument; ids from 0x20 are
 * shifted by the bank offset. */
static inline void Akao_SetSampleInstrument(AkaoTrack *track, u8 id, u32 offset)
{
    u16 instrument = id;

    if (id >= 0x20)
        instrument += offset;
    track->note_pitch = instrument;
    SeqOp_SetVoiceInstrument(track, &D_800B2900[instrument],
                             D_800B2900[instrument].start_address);
}

/* Selects the sample's instrument unless the track already uses it. */
static inline void Akao_SelectSampleInstrument(AkaoTrack *track, u8 *sample, u32 offset)
{
    u32 pitch = track->note_pitch;

    if (sample[0] < 0x20) {
        if (pitch == sample[0])
            return;
    } else if (pitch == sample[0] + offset) {
        return;
    }
    Akao_SetSampleInstrument(track, sample[0], offset);
}

/* Runs a track's command bytes up to the next note: opcodes 0xA0 and up go
 * to the command handlers (0xFC selects the extended page, 0xCA ends the
 * phrase while the track sustains). Then starts the note: its length from
 * the duration table (opcode % 11), and for a key opcode (below 0x84) the
 * pitch from opcode / 11, either through the drum kit sample list or the
 * track's octave, plus LFO restarts. Ties (0x84..0x8E) only continue the
 * portamento; rests (0x8F..) silence the pending effects. */
void Akao_StepSampleLoader(AkaoTrack *track, u32 voice_mask)
{
    u32 opcode;
    u32 value;
    u32 period;
    int flags;
    s16 duration;

    do {
        AkaoCommandHandler handler;

        opcode = *track->pc++;
        if (opcode < 0xA0)
            break;
        if (opcode == 0xFC) {
            period = *track->pc++;
            handler = D_8009CCF0[period];
            handler(track, voice_mask);
        } else {
            if (opcode == 0xCA && (track->flags & AKAO_TRACK_FLAG_KEY_OFF_PENDING)) {
                D_800BCD5C |= voice_mask;
                opcode = 0xA0;
            }
            D_8009C8F0[opcode](track, voice_mask);
        }
    } while (opcode >= 0xA0 && opcode != 0xA0);

    if (opcode == 0xA0) {
        if (track->parent_track_id == 0 && (D_8009D2C8->words[5] & voice_mask)
            && track->assigned_voice_index < 0x18U)
            D_8009D2C8->words[6] |= voice_mask;
        return;
    }

    period = Akao_LookupSampleBankByte(track) & 0xFF;
    duration = track->fixed_note_length;
    if (duration != 0) {
        track->pan_duration = duration;
        track->note_length = duration;
    }
    if (track->note_length != 0) {
        if (period >= 0x8F || (period < 0x84 && !(track->tremolo_phase & 5)))
            track->pan_duration -= 2;
    } else {
        u16 length;

        length = track->note_length = D_8009B8DC[opcode % 11];
        if (period - 0x84 >= 0xB && !(track->tremolo_phase & 5))
            length -= 2;
        track->pan_duration = length;
    }
    track->default_note_length = track->note_length;
    track->update_flags |= 0x4000;
    if (period < 0x8F)
        track->flags &= ~0x40;
    else
        track->flags |= 0x40;

    if (opcode >= 0x8F) {
        if (track->parent_track_id == 0 && (D_8009D2C8->words[5] & voice_mask)
            && track->assigned_voice_index < 0x18U)
            D_8009D2C8->words[6] |= voice_mask;
        track->tremolo_duration = 0;
        track->pitch_lfo_value = 0;
        track->volume_lfo_value = 0;
        track->tremolo_phase &= ~2;
        return;
    }

    if (opcode < 0x84) {
        opcode /= 11;
        flags = track->flags;
        if (flags & AKAO_TRACK_FLAG_BRANCH_ACTIVE) {
            u8 *sample;
            u32 offset;

            D_8009D2C8->words[4] |= voice_mask;
            if ((D_8009D2C8->words[5] & voice_mask)
                && track->assigned_voice_index < 0x18U)
                D_8009D2C8->words[6] |= voice_mask;
            sample = track->branch_target;
            sample += opcode % 12 * 6;
            offset = (D_8009D2C8->words[0] & 0x100) ? 0x30 : 0;
            Akao_SelectSampleInstrument(track, sample, offset);
            period = Akao_LookupPitchPeriod(track->note_pitch, sample[1], track->detune);
            value = (u16)track->volume;
            value = value * (sample[2] + (sample[3] << 8));
            track->expression_value = value << 2;
            track->panpot = ((sample[4] + 0x40) & 0xFF) << 8;
            if (sample[5])
                D_8009D2C8->words[14] |= voice_mask;
            else
                D_8009D2C8->words[14] &= ~voice_mask;
            Seq_MarkTrack38MaskDirty();
        } else {
            opcode += track->panpot_step * 12;
            if (!(track->tremolo_phase & 2)) {
                if (track->parent_track_id == 0) {
                    if (flags & AKAO_TRACK_FLAG_PENDING_NOTE_PITCH)
                        Akao_SetNotePitchBounded(track, opcode);
                    D_8009D2C8->words[4] |= voice_mask;
                    if ((D_8009D2C8->words[5] & voice_mask)
                        && track->assigned_voice_index < 0x18U)
                        D_8009D2C8->words[6] |= voice_mask;
                } else {
                    D_800BCD54 |= voice_mask;
                }
                track->pitch_slide_steps = 0;
            }
            if (track->tremolo_duration != 0 && track->tremolo_counter != 0) {
                track->vibrato_duration = track->tremolo_duration;
                track->vibrato_delta = track->expression + opcode - track->tremolo_counter
                                     - track->tremolo_delta;
                track->current_note = track->tremolo_counter
                                - (track->expression - track->tremolo_delta);
                opcode = track->tremolo_counter + track->tremolo_delta;
            } else {
                track->current_note = opcode;
                opcode += track->expression;
            }
            period = Akao_LookupPitchPeriod(track->note_pitch, opcode, track->detune);
        }

        track->pitch_base = period;
        if (track->parent_track_id == 0)
            D_8009D2C8->words[5] |= voice_mask;
        else
            D_800BCD58 |= voice_mask;
        track->update_flags |= 0x13;
        opcode = track->flags;
        if (opcode & AKAO_TRACK_FLAG_PITCH_LFO) {
            u32 target = track->pitch_lfo_target;
            u32 depth = (target & 0x7F00) >> 8;
            u32 lfo_depth;

            if (!(target & 0x8000))
                lfo_depth = depth * (((period << 4) - period) >> 8) >> 7;
            else
                lfo_depth = depth * period >> 7;
            track->pitch_lfo_depth = lfo_depth;
            track->pitch_lfo_table = D_8009C080[track->pitch_lfo_selector];
            track->pitch_lfo_counter = track->pitch_lfo_delay;
            track->pitch_lfo_phase = 1;
        }
        if (opcode & AKAO_TRACK_FLAG_VOLUME_LFO) {
            track->volume_lfo_table = D_8009C080[track->volume_lfo_selector];
            track->volume_lfo_counter = track->volume_lfo_delay;
            track->volume_lfo_phase = 1;
        }
        if (opcode & AKAO_TRACK_FLAG_AUX_LFO) {
            track->aux_lfo_table = D_8009C080[track->aux_lfo_selector];
            track->aux_lfo_phase = 1;
        }
        track->pitch_lfo_value = 0;
        track->volume_lfo_value = 0;
        track->voice_mask_a = 0;
    }

    track->tremolo_phase = (track->tremolo_phase & ~2) | ((track->tremolo_phase & 1) << 1);
    if (track->vibrato_delta != 0) {
        s16 pitch = track->current_note + track->vibrato_delta;
        u16 steps;
        int slope;

        track->current_note = pitch;
        period = Akao_LookupPitchPeriod(track->note_pitch, pitch + track->expression,
                                        track->detune) << 16;
        steps = track->vibrato_duration;
        slope = (int)(period - ((track->pitch_base << 16) + track->voice_mask_a)) / steps;

        track->vibrato_delta = 0;
        track->pitch_slide_steps = steps;
        track->pitch_slide_step = slope;
    }
    track->tremolo_counter = track->current_note;
    track->tremolo_delta = track->expression;
}
