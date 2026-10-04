#include "pe1/akao/tick.h"

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
