#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/globals.h"
#include "pe1/akao/note_step.h"
#include "pe1/akao/spu_common.h"
#include "pe1/akao/tick.h"
#include "pe1/akao/track.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/akao/voice_state.h"
#include "pe1/akao_counters.h"
#include "pe1/akao_script.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_spu_internal.h"

void Seq_GetGlobalPitch(unsigned int *out)
{
    *out = g_SpuReverbState.mode;
}

void Seq_ApplyGlobalPitch(void) {
    unsigned short value = g_AkaoCdInputVolume;
    SpuCommonAttr *regs;

    regs = &g_AkaoSpuCommonAttr;
    regs->mask = 0x1C0;
    g_AkaoSpuCommonAttr.cd.reverb = 0;
    g_AkaoSpuCommonAttr.cd.volume.right = value;
    g_AkaoSpuCommonAttr.cd.volume.left = value;
    SpuSetCommonAttr(regs);
}

void Akao_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size)
{
    size >>= 2;
    do {
        *dst = *src;
        src++;
        size--;
        dst++;
    } while (size != 0);
}

/* The bank cursor is one field of the sequencer state block. Read as a
 * member, the reload after the bank store stays a separate load: GCC 2.7.2
 * lets a structure store clobber structure reads but not scalar ones. */
typedef struct AkaoBankCursor {
    AkaoTickBank *bank;
} AkaoBankCursor;

/* Advance global, bank and voice slides every fourth tick.
 * Each slide keeps its own previous value so stock GCC can allocate its
 * registers without a pin. */
void Spu_StepReverbLoad(void)
{
    u32 value;
    u32 oldGlobalParam;
    u32 oldPrimaryPitch;
    u32 oldSecondaryPitch;
    u32 oldPanTarget;
    u32 oldPanpot;
    u32 oldPitchOffset;
    u32 pending;
    int count;
    AkaoTrack *track;
    AkaoSequencerBank *bank;
    if ((++g_AkaoGlobalSlideTick & 3) != 0)
        return;
    if (g_AkaoGlobalPitchSlideCounter != 0) {
        g_AkaoGlobalPitchSlideCounter--;
        g_AkaoCdInputVolumeAccum += g_AkaoGlobalPitchSlideStep;
        Seq_ApplyGlobalPitch();
    }
    if (g_AkaoGlobalD2D0SlideCounter != 0) {
        g_AkaoGlobalD2D0SlideCounter--;
        g_AkaoTempoScaleAccum += g_AkaoGlobalD2D0SlideStep;
    }
    if (g_AkaoGlobalD2CCSlideCounter != 0) {
        oldGlobalParam = g_AkaoGlobalPitchDepth;
        g_AkaoGlobalD2CCSlideCounter--;
        value = oldGlobalParam + g_AkaoGlobalD2CCSlideStep;
        if ((value & 0xFF0000) != (oldGlobalParam & 0xFF0000)) {
            track = (AkaoTrack *)g_AkaoSequencerTracks;
            for (count = 24; count != 0; --count, ++track)
                track->update_flags |= 0x10;
        }
        g_AkaoGlobalPitchDepth = value;
    }
    bank = (AkaoSequencerBank *)D_8009CD88;
    if (bank->active_voice_mask && bank->pitch_slide_duration) {
        oldPrimaryPitch = bank->pitch_current;
        bank->pitch_slide_duration--;
        value = oldPrimaryPitch + bank->pitch_delta;
        if ((value & 0x7F0000) != (oldPrimaryPitch & 0x7F0000))
            Seq_MarkDirtyTracks((AkaoTrack *)g_AkaoSequencerTracks);
        ((AkaoSequencerBank *)D_8009CD88)->pitch_current = value;
        bank = (AkaoSequencerBank *)((AkaoBankCursor *)&D_8009CD88)->bank;
    }
    D_8009CD88 = (AkaoTickBank *)(bank + 1);
    if (bank[1].active_voice_mask && bank[1].pitch_slide_duration) {
        oldSecondaryPitch = bank[1].pitch_current;
        bank[1].pitch_slide_duration--;
        value = oldSecondaryPitch + bank[1].pitch_delta;
        if ((value & 0x7F0000) != (oldSecondaryPitch & 0x7F0000))
            Seq_MarkDirtyTracks(g_AkaoVoiceStateTable2);
        ((AkaoSequencerBank *)D_8009CD88)->pitch_current = value;
    }
    D_8009CD88 = D_8009CD88 - 1;
    pending = g_SpuActiveVoiceMask;
    if (pending) {
        count = 0x1000;
        track = g_AkaoVoiceChannelTable;
        do {
            if (pending & count) {
                if (track->panpot_duration) {
                    oldPanTarget = track->pan_target;
                    track->panpot_duration--;
                    value = oldPanTarget + track->pan_delta;
                    if ((value & 0xFF00) != (oldPanTarget & 0xFF00))
                        track->update_flags |= 3;
                    track->pan_target = value;
                }
                if (track->panpot_slide_duration) {
                    oldPanpot = track->panpot;
                    track->panpot_slide_duration--;
                    value = oldPanpot + track->panpot_delta;
                    if ((value & 0xFF00) != (oldPanpot & 0xFF00))
                        track->update_flags |= 3;
                    track->panpot = value;
                }
                if (track->pitch_offset_duration) {
                    oldPitchOffset = track->pitch_offset;
                    track->pitch_offset_duration--;
                    value = oldPitchOffset + track->pitch_offset_step;
                    if ((value & 0xFF00) != (oldPitchOffset & 0xFF00))
                        track->update_flags |= 0x10;
                    track->pitch_offset = value;
                }
                pending ^= count;
            }
            count = count << 1;
            ++track;
        } while (pending);
    }
}

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
    if (D_8009CD88->timing.active_voice_mask) {
        scale = g_AkaoTempoScale;
        tempo = D_8009CD88->timing.tempo.half.integer;
        if (scale) {
            if (scale < 0x80)
                tempo += (tempo * scale) >> 7;
            else
                tempo = (tempo * scale) >> 8;
        }
        D_8009CD88->timing.tick_accumulator += tempo;
        if ((D_8009CD88->timing.tick_accumulator & 0xFFFF0000) || (g_AkaoSeqPendingFlags & 4)) {
            D_8009CD88->timing.tick_accumulator &= 0xFFFF;
            do {
                bit = 1;
                track = g_AkaoSequencerTracks;
                mask = D_8009CD88->timing.active_voice_mask;
                do {
                    if (mask & bit) {
                        track->timing.note_length--;
                        track->timing.gate_length--;
                        if (track->timing.note_length == 0) {
                            Akao_StepSampleLoader(&track->track, bit);
                        } else if (track->timing.gate_length == 0) {
                            D_8009CD88->timing.key_off_request_mask |= bit;
                            D_8009CD88->timing.allocated_voice_mask &= ~bit;
                        }
                        Spu_UpdateVoiceRegisters(&track->track, bit);
                        mask ^= bit;
                    }
                    track++;
                    bit <<= 1;
                } while (mask);
                if (D_8009CD88->timing.tempo_slide_duration) {
                    D_8009CD88->timing.tempo_slide_duration--;
                    D_8009CD88->timing.tempo.value += D_8009CD88->timing.tempo_delta;
                }
                if (D_8009CD88->timing.volume_slide_duration) {
                    /* Updated through scalar pointers: retail keeps the
                     * update-flag read below both stores. */
                    AkaoU16 *duration = &D_8009CD88->timing.volume_slide_duration;
                    AkaoU32 *volume = &D_8009CD88->timing.volume;

                    (*duration)--;
                    *volume += D_8009CD88->timing.volume_delta;
                    g_AkaoVoiceUpdateFlags |= 0x80;
                }
                if (D_8009CD88->timing.ticks_per_beat) {
                    if (++D_8009CD88->timing.tick == D_8009CD88->timing.ticks_per_beat) {
                        D_8009CD88->timing.tick = 0;
                        if (++D_8009CD88->timing.beat == D_8009CD88->timing.beats_per_measure) {
                            D_8009CD88->timing.beat = 0;
                            D_8009CD88->timing.measure++;
                            if (g_AkaoSeqLoopCounter)
                                g_AkaoSeqLoopCounter--;
                        }
                    }
                }
            } while (g_AkaoSeqLoopCounter);
        }
    }

    if (D_8009CD88[1].timing.active_voice_mask) {
        D_8009CD88 = D_8009CD88 + 1;
        tempo = D_8009CD88->timing.tempo.half.integer;
        scale = g_AkaoTempoScale;
        if (scale) {
            if (scale < 0x80)
                tempo += (tempo * scale) >> 7;
            else
                tempo = (tempo * scale) >> 8;
        }
        D_8009CD88->timing.tick_accumulator += tempo;
        if ((D_8009CD88->timing.tick_accumulator & 0xFFFF0000) || (g_AkaoSeqPendingFlags & 4)) {
            D_8009CD88->timing.tick_accumulator &= 0xFFFF;
            bit = 1;
            track = &g_AkaoSequencerTracks[24];
            mask = D_8009CD88->timing.active_voice_mask;
            do {
                if (mask & bit) {
                    track->timing.note_length--;
                    track->timing.gate_length--;
                    if (track->timing.note_length == 0) {
                        Akao_StepSampleLoader(&track->track, bit);
                    } else if (track->timing.gate_length == 0) {
                        D_8009CD88->timing.key_off_request_mask |= bit;
                        D_8009CD88->timing.allocated_voice_mask &= ~bit;
                    }
                    Spu_UpdateVoiceRegisters(&track->track, bit);
                    mask ^= bit;
                }
                track++;
                bit <<= 1;
            } while (mask);
            if (D_8009CD88->timing.tempo_slide_duration) {
                D_8009CD88->timing.tempo_slide_duration--;
                D_8009CD88->timing.tempo.value += D_8009CD88->timing.tempo_delta;
            }
            if (D_8009CD88->timing.volume_slide_duration) {
                D_8009CD88->timing.volume_slide_duration--;
                D_8009CD88->timing.volume += D_8009CD88->timing.volume_delta;
            }
            if (D_8009CD88->timing.ticks_per_beat) {
                if (++D_8009CD88->timing.tick == D_8009CD88->timing.ticks_per_beat) {
                    D_8009CD88->timing.tick = 0;
                    if (++D_8009CD88->timing.beat == D_8009CD88->timing.beats_per_measure) {
                        D_8009CD88->timing.beat = 0;
                        D_8009CD88->timing.measure++;
                        if (g_AkaoSeqLoopCounter)
                            g_AkaoSeqLoopCounter--;
                    }
                }
            }
        }
        D_8009CD88 = D_8009CD88 - 1;
    }

    if (!D_8009CD88->timing.active_voice_mask && !D_8009CD88->timing.pending_restore_mask
        && D_8009CD88[1].timing.active_voice_mask) {
        Akao_CopyWords(D_8009CD88[1].words, D_8009CD88->words, sizeof(AkaoTickBank));
        Akao_CopyWords(g_AkaoSequencerTracks[24].words, g_AkaoSequencerTracks[0].words, sizeof(AkaoTickTrack) * 24);
        D_8009CD88[1].timing.bank_id = 0;
        D_8009CD88[1].timing.active_voice_mask = 0;
    }

    mask = g_SpuActiveVoiceMask;
    if (mask) {
        accumulator = g_AkaoSfxTickAccumulator + g_AkaoSfxTempo.half.integer;
        g_AkaoSfxTickAccumulator = accumulator;
        if ((accumulator & 0xFFFF0000) || (g_AkaoSeqPendingFlags & 4)) {
            g_AkaoSfxTickAccumulator = accumulator & 0xFFFF;
            bit = 0x1000;
            track = (AkaoTickTrack *)g_AkaoVoiceChannelTable;
            do {
                if (mask & bit) {
                    if (!(g_AkaoSeqPendingFlags & 2) || (track->timing.key_on_mask & 0x2000000)) {
                        track->timing.note_length--;
                        track->timing.tick_count++;
                        track->timing.gate_length--;
                        if (track->timing.note_length == 0) {
                            Akao_StepSampleLoader(&track->track, bit);
                        } else if (track->timing.gate_length == 0) {
                            g_SpuPendingKeyOffMask |= bit;
                            g_AkaoSfxAllocatedVoiceMask &= ~bit;
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

    if (!g_AkaoMessageQueueLock)
        Akao_ProcessMessageQueue();
    Spu_StepReverbLoad();
    Akao_SetVoicePitch();
}

/* Timer callback immediately follows the sequencer tick and measures its
 * duration to maintain the rolling AKAO timer history. */

/* Timer event handler (OpenEvent takes an int-returning handler): runs one
 * sequencer tick, records how long it took and returns that duration. */
int Akao_TimerCallback(void) {
    s32 delta;
    s32 old_f0;
    s32 old_f4;
    s32 old_f8;

    delta = GetRCnt(0xF2000002);
    Akao_Tick();
    delta = GetRCnt(0xF2000002) - delta;
    if (delta <= 0) {
        delta += 0x44E8;
    }
    old_f0 = g_AkaoTimerDeltaHist2;
    old_f4 = g_AkaoTimerDeltaHist1;
    old_f8 = g_AkaoTimerDeltaHist0;
    g_AkaoTimerDeltaHist0 = delta;
    g_AkaoTimerDeltaHist3 = old_f0;
    old_f0 = old_f0 + old_f4;
    old_f0 = old_f0 + old_f8;
    old_f0 = old_f0 + delta;

    g_AkaoTimerDeltaHist2 = old_f4;
    g_AkaoTimerDeltaHist1 = old_f8;
    g_AkaoTimerDeltaSum = old_f0;
    return delta;
}

/* Argument lengths of opcodes 0xA0-0xFF, zero for opcodes handled below. */

int Akao_LookupSampleBankByte(AkaoTrack *track)
{
    AkaoU8 *cursor = track->pc;
    unsigned repeat = track->call_stack_index;
    unsigned code;
    int argument;
    int displacement;
    unsigned bank_byte;
    unsigned skip;

    for (;;) {
        code = *cursor;
        for (;;) {
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
            skip = g_AkaoOpcodeArgLengths[code - 0xA0];
            if (skip) {
                cursor += skip;
                break;
            }
            switch (code) {
            case 0xFC:
                cursor++;
                argument = *cursor;
                skip = g_AkaoExtOpcodeArgLengths[argument];
                if (skip) {
                    cursor += skip;
                    break;
                }
                if (argument != 7) {
                    if (argument < 8) {
                        if (argument != 6) {
                            /* Re-dispatch the sub-opcode as a plain opcode. */
                            code = argument;
                            continue;
                        }
                    } else {
                        if (argument < 10) {
                            cursor++;
                            if (*cursor == track->repeat_counters[repeat] + 1) {
                                cursor++;
                                argument = *cursor++;
                                displacement = *cursor++;
                                repeat--;
                    repeat &= 3;
                                argument += displacement << 8;
                                displacement = (short)argument;
                                cursor += displacement;
                            } else {
                                cursor += 3;
                            }
                        }
                        break;
                    }
                    cursor++;
                    argument = *cursor++;
                    displacement = *cursor++;
                    argument += displacement << 8;
                    displacement = (short)argument;
                    cursor += displacement;
                    break;
                }
                cursor++;
                bank_byte = *cursor;
                cursor++;
                if (D_8009CD88->bank.condition_value >= bank_byte) {
                    argument = *cursor++;
                    displacement = *cursor++;
                    argument += displacement << 8;
                    displacement = (short)argument;
                    cursor += displacement;
                } else {
                    cursor += 2;
                }
                break;
            case 0xC9:
                cursor++;
                if (*cursor == track->repeat_counters[repeat] + 1) {
                    cursor++;
                    repeat--;
                    repeat &= 3;
                } else {
                    cursor = track->call_stack[repeat];
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
                    track->tremolo_duration = 0;
                    track->tremolo_phase &= ~5;
                    return 0xA0;
                }
                cursor = track->call_stack[repeat];
                break;
            default:
                track->tremolo_duration = 0;
                track->tremolo_phase &= ~5;
                return 0xA0;
            }
            break;
        }
    }
}
#define NULL ((void *)0)


void Akao_SetVoiceLoopAddr(AkaoTrack *track, u32 arg1) {
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
    temp_a2 = -((g_AkaoCurTrack->status_flags & 0x100) != 0) & 0x30;
    while (var_s0[1].note < 0x80U) {
        var_s0 += 1;
        var_a0 += 1;
    }
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
    var_v1 = 0;
    if (var_a0 != 0) {
        while (1) {
            var_v1 += 1;
            if (var_s0->max_value < arg1) {
                var_s0 += 1;
                if (var_v1 < var_a0) {
                    continue;
                }
            }
            break;
        }
    }
    temp_v1 = var_s0->note;
    temp_a0 = track->note_pitch;
    if ((temp_v1 < 0x20U) ? (temp_a0 != temp_v1) : (temp_a0 != (temp_v1 + temp_a2))) {
        temp_v1_2 = var_s0[1].note;
        temp_a0_2 = track->note_pitch;
        if ((temp_v1_2 < 0x20U) ? (temp_a0_2 != temp_v1_2) : (temp_a0_2 != (temp_v1_2 + temp_a2))) {
            temp_v0 = var_s0->note;
            var_a1 = temp_v0 & 0xFF;
            if (temp_v0 >= 0x20U) {
                var_a1 += temp_a2;
            }
            track->note_pitch = var_a1;
            SeqOp_SetVoiceInstrument(track, &g_AkaoInstrumentTable[var_a1],
                                     g_AkaoInstrumentTable[var_a1].start_address);
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
    s32 last_index;
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
    temp_a2 = -((g_AkaoCurTrack->status_flags & 0x100) != 0) & 0x30;
    while (var_s0->note < 0x80U) {
        var_s0 += 1;
        count_or_note += 1;
    }
    var_s0 = (AkaoVoiceLoopRange *)track->repeat_target;
    var_v1 = count_or_note;
    last_index = var_v1 - 1;
    var_s0 += last_index;
    if (var_v1 != 0) {
        while (1) {
            if (arg1 < var_s0->min_value) {
                var_v1 -= 1;
                var_s0 -= 1;
                if (var_v1 != 0) {
                    continue;
                }
            }
            break;
        }
    }
    count_or_note = var_s0->note;
    temp_a1 = track->note_pitch;
    if ((count_or_note < 0x20U) ? (temp_a1 != count_or_note) : (temp_a1 != (count_or_note + temp_a2))) {
        if (var_v1 != 0) {
            temp_v1 = var_s0[-1].note;
            temp_a0_2 = track->note_pitch;
        }
        if (var_v1 == 0 || ((temp_v1 < 0x20U) ? (temp_a0_2 != temp_v1) : (temp_a0_2 != (temp_v1 + temp_a2)))) {
            temp_v0 = var_s0->note;
            var_a1 = temp_v0 & 0xFF;
            if (temp_v0 >= 0x20U) {
                var_a1 += temp_a2;
            }
            track->note_pitch = var_a1;
            SeqOp_SetVoiceInstrument(track, &g_AkaoInstrumentTable[var_a1],
                                     g_AkaoInstrumentTable[var_a1].start_address);
            track->adsr_attack_rate = var_s0->adsr_attack_rate;
            track->adsr_sustain_rate = var_s0->adsr_sustain_rate;
            track->adsr_sustain = var_s0->adsr_sustain_mode;
            tmp6 = var_s0->adsr_release_rate;
            track->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE_RATE;
            track->adsr_release_rate = tmp6;
        }
    }
}

void Akao_SetNotePitchBounded(AkaoTrack *track, int arg1) {
    int value = track->current_note;

    if ((unsigned int)value < (unsigned int)arg1) {
        Akao_SetVoiceLoopAddr(track, arg1);
    } else if ((unsigned int)arg1 < (unsigned int)value) {
        Akao_SetVoiceLoopAddrAlt(track, arg1);
    }
}

int Akao_LookupPitchPeriod(int instrument, int note, int detune) {
    int offset;
    int octave;
    int shift;
    int period;
    int row_offset;

    note = (u8)note;
    row_offset = instrument << 6;
    period = (int)g_AkaoPitchPeriodTable;
    octave = (unsigned int)note / 12;
    offset = note - (octave * 12);
    offset <<= 2;
    row_offset += period;
    offset += row_offset;
    period = *(int *)offset;
    shift = octave;

    /* Both paths leave the octave in its own register for the upward shift,
     * so the copy is restated on the detune path. */
    if (detune != 0) {
        period += (unsigned int)(period * detune) >> 7;
        shift = octave;
    }

    if ((unsigned int)shift >= 7) {
        offset = shift - 6;
        period <<= offset;
    } else if ((unsigned int)octave < 6) {
        offset = 6 - octave;
        period = (unsigned int)period >> offset;
    }
    return period & 0xFFFF;
}

/* Switches the track to a drum kit sample's instrument; ids from 0x20 are
 * shifted by the bank offset. */
static inline void Akao_SetSampleInstrument(AkaoTrack *track, u8 id, u32 offset)
{
    u16 instrument = id;

    if (id >= 0x20)
        instrument += offset;
    track->note_pitch = instrument;
    SeqOp_SetVoiceInstrument(track, &g_AkaoInstrumentTable[instrument],
                             g_AkaoInstrumentTable[instrument].start_address);
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
            handler = g_AkaoExtOpcodeHandlers[period];
            handler(track, voice_mask);
        } else {
            if (opcode == 0xCA && (track->flags & AKAO_TRACK_FLAG_KEY_OFF_PENDING)) {
                g_SpuPendingKeyOffMask |= voice_mask;
                opcode = 0xA0;
            }
            g_AkaoOpcodeHandlers[opcode](track, voice_mask);
        }
    } while (opcode >= 0xA0 && opcode != 0xA0);

    if (opcode == 0xA0) {
        if (track->parent_track_id == 0 && (D_8009CD88->words[5] & voice_mask)
            && track->assigned_voice_index < 0x18U)
            D_8009CD88->words[6] |= voice_mask;
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

        length = track->note_length = g_AkaoNoteLengthTable[opcode % 11];
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
        if (track->parent_track_id == 0 && (D_8009CD88->words[5] & voice_mask)
            && track->assigned_voice_index < 0x18U)
            D_8009CD88->words[6] |= voice_mask;
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

            D_8009CD88->words[4] |= voice_mask;
            if ((D_8009CD88->words[5] & voice_mask)
                && track->assigned_voice_index < 0x18U)
                D_8009CD88->words[6] |= voice_mask;
            sample = track->branch_target;
            sample += opcode % 12 * 6;
            offset = (D_8009CD88->words[0] & 0x100) ? 0x30 : 0;
            Akao_SelectSampleInstrument(track, sample, offset);
            period = Akao_LookupPitchPeriod(track->note_pitch, sample[1], track->detune);
            value = (u16)track->volume;
            value = value * (sample[2] + (sample[3] << 8));
            track->expression_value = value << 2;
            track->panpot = ((sample[4] + 0x40) & 0xFF) << 8;
            if (sample[5])
                D_8009CD88->words[14] |= voice_mask;
            else
                D_8009CD88->words[14] &= ~voice_mask;
            Seq_MarkTrack38MaskDirty();
        } else {
            opcode += track->panpot_step * 12;
            if (!(track->tremolo_phase & 2)) {
                if (track->parent_track_id == 0) {
                    if (flags & AKAO_TRACK_FLAG_PENDING_NOTE_PITCH)
                        Akao_SetNotePitchBounded(track, opcode);
                    D_8009CD88->words[4] |= voice_mask;
                    if ((D_8009CD88->words[5] & voice_mask)
                        && track->assigned_voice_index < 0x18U)
                        D_8009CD88->words[6] |= voice_mask;
                } else {
                    g_AkaoVoiceMaskScratch |= voice_mask;
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
            D_8009CD88->words[5] |= voice_mask;
        else
            g_AkaoSfxAllocatedVoiceMask |= voice_mask;
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
            track->pitch_lfo_table = g_AkaoSoundEntryTable[track->pitch_lfo_selector];
            track->pitch_lfo_counter = track->pitch_lfo_delay;
            track->pitch_lfo_phase = 1;
        }
        if (opcode & AKAO_TRACK_FLAG_VOLUME_LFO) {
            track->volume_lfo_table = g_AkaoSoundEntryTable[track->volume_lfo_selector];
            track->volume_lfo_counter = track->volume_lfo_delay;
            track->volume_lfo_phase = 1;
        }
        if (opcode & AKAO_TRACK_FLAG_AUX_LFO) {
            track->aux_lfo_table = g_AkaoSoundEntryTable[track->aux_lfo_selector];
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



void SeqOp_SetVoiceInstrument(AkaoTrack *track_arg, AkaoInstrument *instrument_arg, int sample_header) {
    AkaoTrack *track = track_arg;
    AkaoInstrument *instrument = instrument_arg;
    int flags;
    int value;
    u32 flags2;
    int tail_value;

    track->sample_header = sample_header;
    track->sample_data = instrument->loop_address;
    track->adsr_attack_rate = instrument->adsr_attack_rate;
    track->adsr_decay_rate = instrument->adsr_decay_rate;
    track->adsr_sustain_level = instrument->adsr_sustain_level;
    track->adsr_sustain_rate = instrument->adsr_sustain_rate;
    track->adsr_attack = instrument->adsr_attack_mode;

    flags = track->flags;
    value = instrument->adsr_sustain_mode;
    track->adsr_sustain = value;

    if ((flags & AKAO_TRACK_FLAG_RELEASE_RATE_OVERRIDE) != 0) {
        track->update_flags |= AKAO_VOICE_PARAM_INSTRUMENT_NO_RELEASE;
    } else {
        unsigned int v = instrument->adsr_release_rate;
        AkaoU16 *dst = &track->adsr_release_rate;

        *dst = v;
        flags2 = track->update_flags;
        tail_value = instrument->adsr_release_mode;
        track->update_flags = flags2 | AKAO_VOICE_PARAM_INSTRUMENT_FULL;
        track->adsr_release = tail_value;
    }
}

void Akao_SetNotePitch(AkaoTrack *track, int arg1) {
    AkaoInstrument *instrument;

    track->note_pitch = arg1;
    instrument = &g_AkaoInstrumentTable[arg1];
    SeqOp_SetVoiceInstrument(track, instrument, instrument->start_address);
}

void SeqOp_DeactivateVoice(AkaoTrack *ptr, int mask) {
    int *first = &g_SpuActiveVoiceMask;
    AkaoU32 *key_on = &ptr->key_on_mask;
    AkaoU32 *key_off = &ptr->key_off_mask;

    mask = ~mask;
    *first &= mask;
    g_AkaoTrack34Mask &= mask;
    g_AkaoTrack38Mask &= mask;
    g_AkaoTrack3CMask &= mask;
    g_AkaoVoiceMaskScratch &= mask;
    *key_on = 0;
    *key_off = 0;
}

void SeqOp_SetTempo(void *ptr)
{
  unsigned char *stream = *((unsigned char **) ptr);
  AkaoSequencerBank *track;
  register unsigned int value;
  *((unsigned char **) ptr) = stream + 1;
  track = g_AkaoCurTrack;
  ;
  track->tempo = (*stream) << 16;
  stream = *((unsigned char **) ptr);
  *((unsigned char **) ptr) = stream + 1;
  stream = (unsigned char *) ((unsigned int) (*stream));
  value = track->tempo;
  track->tempo_slide_duration = 0;
  value |= ((unsigned int) stream) << 24;
  track->tempo = value;
}

void SeqOp_SetPitchSlideTarget(void *ptr) {
    u8 *pc;

    {
        int value;
        u8 *pc0;
        AkaoSequencerBank *track;

        pc0 = *(u8 **)ptr;
        *(u8 **)ptr = pc0 + 1;
        track = g_AkaoCurTrack;
        value = (track->tempo_slide_duration = *pc0);
        if (value == 0) {
            value = 0x100;
            track->tempo_slide_duration = value;
        }
    }

    {
        int value;
        int high;
        AkaoSequencerBank *track;
        int current;

        pc = *(u8 **)ptr;
        *(u8 **)ptr = pc + 1;
        value = pc[0];
        *(u8 **)ptr = pc + 2;
        high = pc[1];

        track = g_AkaoCurTrack;
        value <<= 16;
        high <<= 24;
        value |= high;

        current = track->tempo;
        current &= 0xFFFF0000;
        track->tempo = current;
        track->tempo_delta = (value - current) / track->tempo_slide_duration;
    }
}

void SeqOp_SetTempoSlide(void *ptr)
{
    unsigned char *stream = *(unsigned char **)ptr;
    AkaoSequencerBank *track;
    unsigned int value;

    *(unsigned char **)ptr = stream + 1;
    value = *stream << 16;
    *(unsigned char **)ptr = stream + 2;
    track = g_AkaoCurTrack;
    value |= stream[1] << 24;
    {
        AkaoU16 *slide = &track->volume_slide_duration;
        AkaoU32 *volume = &track->volume;

        *slide = 0;
        g_AkaoVoiceUpdateFlags |= AKAO_GLOBAL_UPDATE_TEMPO;
        *volume = value;
    }
}

static inline u8 **SetDuration(u8 **cursor) {
    u8 *pc = *cursor;
    AkaoSequencerBank *track;
    int value;

    *cursor = pc + 1;
    track = g_AkaoCurTrack;
    value = pc[0];
    track->volume_slide_duration = value;
    if (value == 0) {
        track->volume_slide_duration = 0x100;
    }
    return cursor;
}

void SeqOp_SetVolumeSlideTarget(void *ptr) {
    void *stream;
    u8 *pc;
    int value;

    stream = SetDuration((u8 **)ptr);

    {
        int high;
        AkaoSequencerBank *track;
        int current;

        pc = *(u8 **)stream;
        *(u8 **)stream = pc + 1;
        value = pc[0];
        *(u8 **)stream = pc + 2;
        high = pc[1];

        track = g_AkaoCurTrack;
        value <<= 16;
        high <<= 24;
        value |= high;

        current = track->volume;
        current &= 0xFFFF0000;
        track->volume = current;
        track->volume_delta = (value - current) / track->volume_slide_duration;
    }
}

#define READ_S16(addr) ((short)(*(addr)++ | (*(addr)++ << 8)))

void SeqOp_JumpRelative(unsigned char **stream) {
    *stream += READ_S16(*stream);
}

void SeqOp_BranchIfLoopLimitReached(AkaoTrack *track)
{
    AkaoU8 *pc;
    AkaoU8 *target;
    int value;
    int lo;

    pc = track->pc;
    track->pc = pc + 1;
    value = pc[0];
    if (g_AkaoCurTrack->condition_value >= value) {
        track->pc = pc + 2;
        lo = pc[1];
        target = pc + 3;
        track->pc = target;
        target += (short)(lo | (pc[2] << 8));
        track->pc = target;
    } else {
        track->pc = pc + 3;
    }
}

void SeqOp_ReadVolume(AkaoTrack *track) {
    unsigned char *stream = track->pc;
    unsigned int flags;
    int value;

    track->pc = stream + 1;
    flags = track->update_flags;
    value = *stream;
    track->update_flags = flags | AKAO_VOICE_PARAM_VOLUME;
    track->volume_base = value << 8;
}

static inline int ReadVolumeByte(u8 **cursor) {
    u8 *pc = *cursor;
    *cursor = pc + 1;
    return pc[0];
}

void SeqOp_SlideVolume(AkaoTrack *track) {
    u8 *pc;
    int duration;
    int current;
    int next;

    duration = ReadVolumeByte(&track->pc);
    track->volume_duration = duration;
    if (duration == 0) {
        duration = 0x100;
        track->volume_duration = duration;
    }

    pc = track->pc;
    current = track->volume_base & 0x7F00;
    track->pc = pc + 1;
    next = pc[0] << 8;
    track->volume_base = current;
    track->volume_delta = (next - current) / track->volume_duration;
}

void SeqOp_SetVolumeOrExpression(AkaoTrack *track) {
    AkaoU32 new_var;
    register unsigned char *cursor;
    int value;

    if ((track->flags & AKAO_TRACK_FLAG_BRANCH_ACTIVE) != 0) {
        cursor = track->pc;
        track->pc = cursor + 1;
        if (1) {
            value = *cursor;
            track->volume = value << 7;
        }
    } else {
        register unsigned char *cursor2;
        register unsigned char *next;
        register unsigned int flags;
        register int value2;

        cursor2 = track->pc;
        next = cursor2 + 1;
        track->pc = next;
        new_var = track->update_flags;
        value2 = *((signed char *)cursor2);
        flags = new_var;
        track->expression_duration = 0;
        flags |= AKAO_VOICE_PARAM_VOLUME;
        value2 <<= 23;
        track->update_flags = flags;
        track->expression_value = value2;
    }
}




void Akao_SetExpressionSlide(AkaoTrack *track) {
    u8 *pc;
    u8 *pc2;
    int duration;
    int current;
    int next;

    pc = track->pc;
    track->pc = pc + 1;
    duration = (track->expression_duration = *pc);
    if (duration == 0) {
        duration = 0x100;
        track->expression_duration = duration;
    }

    pc2 = track->pc;
    track->pc = pc2 + 1;
    next = ((s8)pc2[0]) << 23;
    current = track->expression_value & 0xFFFF0000;
    track->expression_value = current;
    track->expression_delta = (next - current) / track->expression_duration;
}

void SeqOp_Skip2(int *arg0)
{
    *arg0 += 2;
}

void SeqOp_Ignore(void)
{
}

void Akao_SetPanTarget(AkaoTrack *track)
{
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    value = *cursor;
    flags = track->flags;
    track->panpot_duration = 0;
    track->pan_target = value << 8;

    if (flags & 0x100) {
        track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
    }
}

void Akao_SetPanTargetSlide(AkaoTrack *track) {
    int duration;
    int pan_base;
    int next;
    int delta;

    duration = (track->panpot_duration = *track->pc++);
    if (duration == 0) {
        duration = 0x100;
        track->panpot_duration = duration;
    }

    pan_base = (unsigned short)track->pan_target & 0xFF00;
    next = *track->pc++;
    delta = ((next << 8) - (short)pan_base) / track->panpot_duration;
    track->pan_target = pan_base;
    track->pan_delta = delta;
}

void Akao_SetPanpot(AkaoTrack *track)
{
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    track->panpot_slide_duration = 0;
    value += 0x40;
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
    value &= 0xFF;
    track->panpot = value << 8;
}

void Akao_SetPanSlide(AkaoTrack *track) {
    int duration;
    int panpot_base;
    int next;
    int delta;

    duration = (track->panpot_slide_duration = *track->pc++);
    if (duration == 0) {
        duration = 0x100;
        track->panpot_slide_duration = duration;
    }

    panpot_base = track->panpot & 0xFF00;
    next = *track->pc++;
    delta = ((((next + 0x40) & 0xFF) << 8) - panpot_base) / track->panpot_slide_duration;
    track->panpot = panpot_base;
    track->panpot_delta = delta;
}

void SeqOp_ReadPanpot(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->panpot_step = *cursor;
}

void SeqOp_IncPanpot(AkaoTrack *track) {
    track->panpot_step = (track->panpot_step + 1) & 0xF;
}

void SeqOp_DecPanpot(AkaoTrack *track) {
    track->panpot_step = (track->panpot_step - 1) & 0xF;
}

void Akao_PlayNote(AkaoTrack *track, unsigned int mask) {
    u8 *pc;
    unsigned int note;
    unsigned int old_note;
    AkaoInstrument *instrument;

    pc = track->pc;
    track->pc = pc + 1;
    note = *pc;

    if (track->parent_track_id == 0) {
        if ((g_AkaoCurTrack->status_flags & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    instrument = &g_AkaoInstrumentTable[note];
    old_note = track->note_pitch;
    track->note_pitch = note;
    if (old_note != 0xFF) {
        if (track->parent_track_id == 0) {
            if ((g_AkaoCurTrack->allocated_voice_mask & mask & g_SpuActiveVoiceMask) == 0) {
                track->update_flags |= AKAO_VOICE_PARAM_PITCH;
                track->pitch_base =
                    (track->pitch_base * instrument->pitch[0]) /
                    (unsigned int)g_AkaoPitchPeriodTable[old_note * 16];
            }
        } else {
            track->update_flags |= AKAO_VOICE_PARAM_PITCH;
            track->pitch_base =
                (track->pitch_base * instrument->pitch[0]) /
                (unsigned int)g_AkaoPitchPeriodTable[old_note * 16];
        }
    }

    SeqOp_SetVoiceInstrument(track, instrument, instrument->start_address);
    track->flags &= ~AKAO_TRACK_FLAG_PENDING_NOTE_PITCH;
}

void Akao_TieNote(AkaoTrack *track, unsigned int mask) {
    u8 *pc;
    int note;
    unsigned int old_note;
    AkaoInstrument *instrument;

    pc = track->pc;
    track->pc = pc + 1;
    note = *pc;

    if (track->parent_track_id == 0) {
        if ((g_AkaoCurTrack->status_flags & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    instrument = &g_AkaoInstrumentTable[note];
    old_note = track->note_pitch;
    track->note_pitch = note;
    if (old_note != 0xFF) {
        if (track->parent_track_id == 0) {
            if ((g_AkaoCurTrack->allocated_voice_mask & mask & g_SpuActiveVoiceMask) == 0) {
                track->update_flags |= AKAO_VOICE_PARAM_PITCH;
                track->pitch_base =
                    (track->pitch_base * instrument->pitch[0]) /
                    (unsigned int)g_AkaoPitchPeriodTable[old_note * 16];
            }
        } else {
            track->update_flags |= AKAO_VOICE_PARAM_PITCH;
            track->pitch_base =
                (track->pitch_base * instrument->pitch[0]) /
                (unsigned int)g_AkaoPitchPeriodTable[old_note * 16];
        }
    }

    SeqOp_SetVoiceInstrument(track, instrument, 0x1010);
    track->flags &= ~AKAO_TRACK_FLAG_PENDING_NOTE_PITCH;
}

void SeqOp_SetReturnPoint(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    cursor = track->pc;
    value = *track->pc++;
    next_cursor = cursor + 2;
    track->pc = next_cursor;
    value |= cursor[1] << 8;

    track->note_pitch = 0xFF;
    track->current_note = 0;
    track->flags |= AKAO_TRACK_FLAG_PENDING_NOTE_PITCH;
    track->repeat_target = next_cursor + (short)value;
}

void SeqOp_LoadInstrumentFromIndex(void *ptr)
{
  int value;
  register int base;

  base = ((AkaoTrack *)ptr)->note_pitch;
  base = ((int)&g_AkaoInstrumentTable) + (base << 6);
  value = ((AkaoInstrument *)base)->adsr_attack_rate;
  ((AkaoTrack *)ptr)->adsr_attack_rate = value;
  value = ((AkaoInstrument *)base)->adsr_decay_rate;
  ((AkaoTrack *)ptr)->adsr_decay_rate = value;
  value = ((AkaoInstrument *)base)->adsr_sustain_level;
  ((AkaoTrack *)ptr)->adsr_sustain_level = value;
  value = ((AkaoInstrument *)base)->adsr_sustain_rate;
  ((AkaoTrack *)ptr)->adsr_sustain_rate = value;
  value = ((AkaoInstrument *)base)->adsr_release_rate;
  ((AkaoTrack *)ptr)->adsr_release_rate = value;
  value = ((AkaoInstrument *)base)->adsr_attack_mode;
  ((AkaoTrack *)ptr)->adsr_attack = value;
  value = ((AkaoInstrument *)base)->adsr_sustain_mode;
  ((AkaoTrack *)ptr)->adsr_sustain = value;
  value = ((AkaoTrack *)ptr)->update_flags;
  base = ((AkaoInstrument *)base)->adsr_release_mode;
  value |= AKAO_VOICE_PARAM_ADSR_ALL;
  ((AkaoTrack *)ptr)->update_flags = value;
  ((AkaoTrack *)ptr)->adsr_release = base;
}

void SeqOp_ReadExpression(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->expression = (signed char)*cursor;
}

void SeqOp_AdjustExpression(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->expression += (signed char)*cursor;
}

void SeqOp_SetVibratoParams(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    cursor = track->pc;
    track->pc = cursor + 1;
    value = *cursor;
    track->vibrato_duration = value;
    if (value == 0) {
        track->vibrato_duration = 0x100;
    }

    next_cursor = track->pc;
    track->pc = next_cursor + 1;
    track->vibrato_delta = (signed char)*next_cursor;
}

void SeqOp_SetTremoloParams(AkaoTrack *track) {
    unsigned char *cursor;
    int value;

    cursor = track->pc;
    track->pc = cursor + 1;
    value = *cursor;
    track->tremolo_duration = value;
    if (value == 0) {
        track->tremolo_duration = 0x100;
    }

    track->tremolo_delta = 0;
    track->tremolo_counter = 0;
    track->tremolo_phase = 1;
}

void SeqOp_StopTremolo(AkaoTrack *track) {
    track->tremolo_duration = 0;
}

void SeqOp_ReadDetune(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->detune = (signed char)*cursor;
}

void SeqOp_AdjustDetune(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->detune += (signed char)*cursor;
}

void SeqOp_SetPitchLFO(void *ptr) {
    AkaoTrack *track;
    AkaoU8 *pc;
    void *table;
    int duration;
    int target;
    int lfo_target;
    unsigned int masked;
    unsigned int depth;
    unsigned int lfo_depth;
    int scale;
    int scaled;
    int use_base_scale;
    int selector;

    track = ptr;
    track->flags |= AKAO_TRACK_FLAG_PITCH_LFO;

    if (track->parent_track_id != 0) {
        pc = track->pc;
        track->pitch_lfo_delay = 0;
        track->pc = pc + 1;
        target = pc[0];
        if (target != 0) {
            track->pitch_lfo_target = target << 8;
        }
    } else {
        AkaoU8 *pc_zero;
        pc_zero = track->pc;
        track->pc = pc_zero + 1;
        track->pitch_lfo_delay = pc_zero[0];
    }

    {
        AkaoU8 *pc_duration;
        pc_duration = track->pc;
        track->pc = pc_duration + 1;
        duration = pc_duration[0];
    }
    track->pitch_lfo_duration = duration;
    if (duration == 0) {
        track->pitch_lfo_duration = 0x100;
    }

    pc = track->pc;
    scale = *(AkaoU16 *)&track->pitch_base;
    track->pc = pc + 1;
    selector = pc[0];

    lfo_target = track->pitch_lfo_target;
    track->pitch_lfo_selector = selector;
    masked = lfo_target & 0x7F00;
    depth = masked >> 8;
    use_base_scale = lfo_target & 0x8000;
    /* Each arm finishes the depth; the identical multiply tails merge at the join. */
    if (use_base_scale == 0) {
        scaled = ((scale << 4) - scale) >> 8;
        lfo_depth = (depth * scaled) >> 7;
    } else {
        lfo_depth = (depth * scale) >> 7;
    }
    track->pitch_lfo_depth = lfo_depth;
    table = g_AkaoSoundEntryTable[track->pitch_lfo_selector];
    track->pitch_lfo_counter = track->pitch_lfo_delay;
    track->pitch_lfo_phase = 1;
    track->pitch_lfo_table = table;
}

void SeqOp_UpdatePitchLFOTarget(AkaoTrack *track) {
    AkaoU8 *pc;
    int value;
    unsigned int masked;
    unsigned int depth;
    int scale;
    int scaled;
    int product;
    int use_base_scale;

    pc = track->pc;
    track->pc = pc + 1;
    value = pc[0];
    value <<= 8;
    masked = value & 0x7F00;
    depth = masked >> 8;
    track->pitch_lfo_target = value;
    use_base_scale = value & 0x8000;
    scale = track->pitch_base;
    if (use_base_scale == 0) {
        scaled = ((scale << 4) - scale) >> 8;
        product = depth * scaled;
    } else {
        product = depth * scale;
    }
    track->pitch_lfo_depth = (unsigned int)product >> 7;
}

void SeqOp_SetPanSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    delta = ((pc[1] << 8) - state->pan_current) / duration;
    state->pan_duration = duration;
    state->pan_delta = delta;
}

void SeqOp_StopPitchLFO(AkaoTrack *track) {
    track->pitch_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_PITCH_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_PITCH;
}

void SeqOp_SetVolumeLFO(AkaoTrack *track) {
    u8 *pc;
    int value;
    int selector;
    int tmp;
    void *entry;

    track->flags |= AKAO_TRACK_FLAG_VOLUME_LFO;

    if (track->parent_track_id != 0) {
        track->volume_lfo_delay = 0;
        pc = track->pc;
        track->pc = pc + 1;
        value = pc[0];
        if (value != 0) {
            track->volume_lfo_target = value << 8;
        }
    } else {
        track->volume_lfo_delay = *track->pc++;
    }

    /* The chained assignment and the *(*pp)++ consume idiom are load-bearing:
     * they keep the stream pointer temp in $v0 as retail allocates it. */
    value = (track->volume_lfo_duration = *track->pc++);
    if (value == 0) {
        track->volume_lfo_duration = 0x100;
    }

    pc = track->pc;
    track->pc = pc + 1;
    selector = pc[0];
    tmp = track->volume_lfo_delay;
    track->volume_lfo_selector = selector;
    entry = g_AkaoSoundEntryTable[selector];
    track->volume_lfo_counter = tmp;
    track->volume_lfo_phase = 1;
    track->volume_lfo_table = entry;
}

void SeqOp_UpdateVolumeLFOTarget(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->volume_lfo_target = *cursor << 8;
}

void SeqOp_SetExpressionSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    delta = ((pc[1] << 8) - state->expr_current) / duration;
    state->expr_duration = duration;
    state->expr_delta = delta;
}

void SeqOp_ResetTrack(AkaoTrack *track) {
    track->volume_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_VOLUME_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
}

void SeqOp_LoadSoundEntry(AkaoTrack *track) {
    u8 *pc;
    int duration;
    int selector;

    track->flags |= AKAO_TRACK_FLAG_AUX_LFO;

    pc = track->pc;
    track->pc = pc + 1;
    duration = pc[0];
    track->aux_lfo_duration = duration;
    if (duration == 0) {
        track->aux_lfo_duration = 0x100;
    }

    pc = track->pc;
    track->pc = pc + 1;
    selector = pc[0];
    track->aux_lfo_selector = selector;
    {
        void *entry = g_AkaoSoundEntryTable[selector];
        track->aux_lfo_phase = 1;
        track->aux_lfo_table = entry;
    }
}

void SeqOp_SetPitchBase(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->aux_lfo_target = *cursor << 7;
}

void SeqOp_PitchSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int target_delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    target_delta = ((pc[1] << 7) - state->pitch_current) / duration;
    state->pitch_duration = duration;
    state->pitch_delta = target_delta;
}


void SeqOp_ResetFlag4State(AkaoTrack *track) {
    track->aux_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_AUX_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
}

void SeqOp_SetTrack34Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_a |= mask;
    } else {
        g_AkaoTrack34Mask |= mask;
    }

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
}

void SeqOp_ClearTrack34Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_a &= ~mask;
    } else {
        g_AkaoTrack34Mask &= ~mask;
    }

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    track->key_on_delay = 0;
}


void SeqOp_SetMask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        (g_AkaoCurTrack)->adsr_dirty_mask |= mask;
    } else if ((track->flags & 0x10000) != 0) {
        g_AkaoTrack3CMask |= mask;
    }

    Seq_MarkTrack3CMaskDirty();
}

void SeqOp_ClearTrack3CMask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        (g_AkaoCurTrack)->adsr_dirty_mask &= ~mask;
    } else {
        g_AkaoTrack3CMask &= ~mask;
    }

    Seq_MarkTrack3CMaskDirty();
    track->key_off_delay = 0;
}


void SeqOp_SetTrack38Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        g_AkaoCurTrack->volume_dirty_mask |= mask;
    } else {
        g_AkaoTrack38Mask |= mask;
    }

    Seq_MarkTrack38MaskDirty();
}

void SeqOp_ClearMask(AkaoTrack *track, u32 mask) {
    AkaoSequencerBank *player;

    if (track->parent_track_id == 0) {
        player = g_AkaoCurTrack;
        player->volume_dirty_mask &= ~mask;
        if ((track->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) != 0) {
            player->reserved_voice_mask &= ~(1 << track->voice_index);
        }
    } else {
        g_AkaoTrack38Mask &= ~mask;
    }

    Seq_MarkTrack38MaskDirty();
}

void SeqOp_EnableField84(AkaoTrack *track) {
    track->tremolo_phase = 1;
}

void SeqOp_Noop_904AC(void) {
}

void SeqOp_Noop_904B4(void) {
}

void SeqOp_Noop_904BC(void) {
}

unsigned int SeqOp_SetTrack5AValue(void *ptr)
{
  u8 *pc;
  int value;
  register AkaoSequencerBank *track;
  pc = *((u8 **) ptr);
  *((u8 **) ptr) = pc + 1;
  value = pc[0];
  if (((AkaoTrack *)ptr)->parent_track_id == 0)
  {
    if ((value & 0xC0) != 0)
    {
      ;
      g_AkaoCurTrack->noise_clock = ((g_AkaoCurTrack->noise_clock) + (value & 0x3F)) & 0x3F;
    }
    else
    {
      track = g_AkaoCurTrack;
      track->noise_clock = value;
    }
  }
  else
    if ((value & 0xC0) != 0)
  {
    int m = value & 0x3F;
    g_AkaoTrack5ATransposeValue = (g_AkaoTrack5ATransposeValue + m) & 0x3F;
  }
  else
  {
    g_AkaoTrack5ATransposeValue = value;
  }
  g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
}

void SeqOp_ReadAdsrAttackRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_ATTACK;
    track->adsr_attack_rate = value;
}

void SeqOp_ReadAdsrDecayRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_DECAY_RATE;
    track->adsr_decay_rate = value;
}

void SeqOp_ReadAdsrSustainLevel(AkaoTrack *track, u32 voice_mask) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL;
    track->adsr_sustain_level = value;
}

void SeqOp_ReadAdsrSustainRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_SUSTAIN;
    track->adsr_sustain_rate = value;
}

void SeqOp_ReadAdsrReleaseRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_RELEASE;
    track->adsr_release_rate = value;
}

void SeqOp_ReadAdsrAttack(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_ATTACK_MODE;
    track->adsr_attack = value;
}

void SeqOp_ReadAdsrSustain(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_SUSTAIN_MODE;
    track->adsr_sustain = value;
}

void SeqOp_ReadAdsrRelease(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_RELEASE_MODE;
    track->adsr_release = value;
}

void Akao_SetReleaseRate(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned int flags;
    unsigned char value;

    track->flags |= AKAO_TRACK_FLAG_RELEASE_RATE_OVERRIDE;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | AKAO_VOICE_PARAM_ADSR_RELEASE;
    track->adsr_release_rate = value;
}

void Akao_ResetReleaseRate(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_RELEASE_RATE_OVERRIDE;
    track->adsr_release_rate = g_AkaoInstrumentTable[track->note_pitch].adsr_release_rate;
    track->update_flags |= AKAO_VOICE_PARAM_ADSR_RELEASE;
}

void SeqOp_PushLoopPoint(AkaoTrack *track) {
    unsigned short index = (track->call_stack_index + 1) & 3;

    track->call_stack_index = index;
    track->call_stack[index] = track->pc;
    index = track->call_stack_index;
    track->repeat_counters[index] = 0;
}

void SeqOp_LoopCounter(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    int value;
    unsigned short index;
    unsigned short counter;

    track->pc = cursor + 1;
    value = *cursor;
    if (value == 0) {
        value = 0x100;
    }

    index = track->call_stack_index;
    counter = track->repeat_counters[index] + 1;
    track->repeat_counters[index] = counter;

    if (counter != value) {
        index = track->call_stack_index;
        track->pc = track->call_stack[index];
    } else {
        index = track->call_stack_index;
        index = (index - 1) & 3;
        track->call_stack_index = index;
    }
}

void SeqOp_JumpIfLoopCount(AkaoTrack *track) {
    AkaoU8 *cursor;
    AkaoU8 *target;
    int count;
    int low;

    cursor = track->pc;
    track->pc = cursor + 1;
    count = cursor[0];
    if (count == 0) {
        count = 0x100;
    }

    if (track->repeat_counters[track->call_stack_index] + 1 != count) {
        track->pc = cursor + 3;
        return;
    }

    track->pc = cursor + 2;
    low = cursor[1];
    target = cursor + 3;
    track->pc = target;
    track->pc = target + (s16)(low | (cursor[2] << 8));
}

void SeqOp_JumpIfLoopCountPop(AkaoTrack *track) {
    AkaoU8 *cursor;
    AkaoU8 *target;
    int count;
    int low;

    cursor = track->pc;
    track->pc = cursor + 1;
    count = cursor[0];
    if (count == 0) {
        count = 0x100;
    }

    if (track->repeat_counters[track->call_stack_index] + 1 != count) {
        track->pc = cursor + 3;
        return;
    }

    track->pc = cursor + 2;
    low = cursor[1];
    target = cursor + 3;
    track->pc = target;
    track->pc = target + (s16)(low | (cursor[2] << 8));
    track->call_stack_index = (track->call_stack_index - 1) & 3;
}

void SeqOp_Return(AkaoTrack *track) {
    unsigned short index = track->call_stack_index;

    track->repeat_counters[index]++;
    track->pc = track->call_stack[track->call_stack_index];
}

void SeqOp_SetVolume(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    track->fixed_note_length = 0;
    track->pan_duration = value;
    track->note_length = value;
    track->default_note_length = value;
}

void SeqOp_AdjustVolumeTarget(AkaoTrack *track) {
    char *cursor = track->pc;
    int value;

    track->pc = (unsigned char *)cursor + 1;
    value = *(signed char *)cursor;
    if (value != 0) {
        value += track->default_note_length;
        if (value <= 0) {
            value = 1;
        } else if (value >= 0x100) {
            value = 0xFF;
        }
    }

    track->fixed_note_length = value;
}

void SeqOp_SetBranchTarget(AkaoScriptState *state) {
    u8 *pc = state->pc;
    u8 *target;
    int saved;
    int lo;
    int hi;
    int offset;

    saved = state->loop_counter;
    state->pc = pc + 1;
    lo = pc[0];
    target = pc + 2;
    state->pc = target;
    hi = pc[1];
    state->loop_counter_saved = saved;
    state->flags |= 8;
    offset = (s16)(lo | (hi << 8));
    state->branch_target = target + offset;
}

void SeqOp_ClearFlag3(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_BRANCH_ACTIVE;
}

void SeqOp_StreamPair(unsigned char **stream) {
    unsigned char *cursor = *stream;
    unsigned char value;
    AkaoTrack *track = (AkaoTrack *)g_AkaoCurTrack;

    *stream = cursor + 1;
    track->pitch_slide_duration = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    value = cursor[0];
    track->repeat_counters[0] = 0;
    track->pitch_slide_current = 0;
    track->voice_index = value;
}

void SeqOp_ReadTrack64U16(unsigned char **stream) {
    unsigned char *cursor = *stream;
    AkaoTrack *track = (AkaoTrack *)g_AkaoCurTrack;

    *stream = cursor + 1;
    track->repeat_counters[1] = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    track->repeat_counters[1] |= cursor[0] << 8;
}

void SeqOp_ReadAdsrDecayAndSustain(AkaoTrack *track, u32 voice_mask) {
    SeqOp_ReadAdsrDecayRate(track);
    SeqOp_ReadAdsrSustainLevel(track, voice_mask);
}

void SeqOp_SetDurationBAAndMask(AkaoTrack *track, u32 voice_mask) {
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    if (value != 0) {
        value++;
    } else {
        value = 0x101;
    }
    track->key_on_delay = value;
    SeqOp_SetTrack34Mask(track, voice_mask);
}

void SeqOp_SetDuration(AkaoTrack *track) {
    unsigned char *ptr = track->pc;
    int value;

    track->pc = ptr + 1;
    value = ptr[0];
    if (value == 0) {
        value = 0x101;
    } else {
        value++;
    }
    track->key_on_delay = value;
}

void SeqOp_SetRepeatCount(AkaoTrack *track, u32 voice_mask) {
    unsigned char *cursor = track->pc;
    int value;

    track->pc = cursor + 1;
    value = *cursor;
    if (value != 0) {
        value++;
    } else {
        value = 0x101;
    }
    track->key_off_delay = value;
    SeqOp_SetMask(track, voice_mask);
}

void SeqOp_SetDurationAlt(AkaoTrack *track) {
    unsigned char *ptr = track->pc;
    int value;

    track->pc = ptr + 1;
    value = ptr[0];
    if (value == 0) {
        value = 0x101;
    } else {
        value++;
    }
    track->key_off_delay = value;
}

void SeqOp_StopAndClearTrack(AkaoTrack *track, u32 voice_mask) {
    track->flags &= ~AKAO_TRACK_FLAG_STOP_CLEAR_MASK;
    SeqOp_ClearTrack34Mask(track, voice_mask);
    SeqOp_ClearTrack3CMask(track, voice_mask);
    SeqOp_ClearMask(track, voice_mask);
    track->tremolo_phase &= 0xFFFA;
}

void SeqOp_SetFlag4(AkaoTrack *track) {
    track->flags |= AKAO_TRACK_FLAG_10;
}

void SeqOp_ClearFlag10(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_10;
}

void SeqOp_SetFlag20(AkaoTrack *track) {
    track->flags |= AKAO_TRACK_FLAG_20;
}

void SeqOp_ClearFlag20(AkaoTrack *track) {
    track->flags &= ~AKAO_TRACK_FLAG_20;
}

void SeqOp_LoadNestedStreams(AkaoTrack *track) {
    u8 *cursor;
    u32 offset;
    u8 *first;
    u8 *second;

    cursor = *(u8 **)track;
    offset = (cursor[1] << 8) | cursor[0];
    if (offset != 0) {
        first = cursor + offset + 2;
    } else {
        first = 0;
    }

    cursor += 2;
    offset = (cursor[1] << 8) | cursor[0];
    if (offset != 0) {
        second = cursor + offset + 2;
    } else {
        second = 0;
    }

    g_AkaoNestedKeyOffMask = 0;
    g_AkaoNestedKeyOnMask = 0;
    {
        AkaoU16 *panpot = &track->panpot;
        int *expression = &track->expression_value;

        g_AkaoNestedPan = *panpot >> 8;
        g_AkaoNestedPanTarget = *expression >> 23;
    }
    Seq_StartNestedStreams(&g_AkaoNestedSource, first, second);
    *(u8 **)track += 4;
}

void SeqOp_NoteOnWithVoiceAlloc(AkaoTrack *track, u32 mask) {
    s16 voice_index;
    s32 voice_mask;
    s32 used_mask;
    s32 valid_mask;
    unsigned char *cursor;

    cursor = track->pc;
    track->pc = cursor + 1;
    track->pitch_slide_current = *cursor << 8;
    track->pitch_slide_duration = 0;
    if (!(track->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED)) {
        voice_index = 0;
        voice_mask = 1;
        valid_mask = 0xFFFFFF;
        used_mask = g_AkaoCurTrack->active_voice_mask | g_AkaoCurTrack->reserved_voice_mask;
        for (;;) {
            if (used_mask & voice_mask) {
                voice_mask *= 2;
                voice_index += 1;
                if (voice_mask & valid_mask) {
                    continue;
                }
            }
            break;
        }
        if (voice_mask & 0xFFFFFF) {
            g_AkaoCurTrack->reserved_voice_mask |= voice_mask;
            track->voice_index = voice_index;
            track->flags |= AKAO_TRACK_FLAG_VOICE_ALLOCATED;
        }
    }
    SeqOp_SetTrack38Mask(track, mask);
}

void SeqOp_NoteOnWithPitchSlide(AkaoTrack *track, u32 voice_mask) {
    AkaoU8 *cursor;
    AkaoU8 *first;
    int step_count;
    int pitch_base;
    int delta;
    int voice_index;
    int mask;
    u32 used_mask;
    u32 valid_mask;

    first = track->pc;
    track->pc = first + 1;
    step_count = first[0];
    track->pitch_slide_duration = step_count;
    if (step_count == 0) {
        track->pitch_slide_duration = 0x100;
    }

    cursor = track->pc;
    pitch_base = track->pitch_slide_current & 0xFF00;
    track->pc = cursor + 1;
    delta = ((cursor[0] << 24) >> 16) - pitch_base;
    track->pitch_slide_current = pitch_base;
    track->pitch_slide_delta = delta / track->pitch_slide_duration;

    if ((track->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) == 0) {
        voice_index = 0;
        mask = 1;
        valid_mask = 0xFFFFFF;
        used_mask = D_8009CD88->bank.active_voice_mask | D_8009CD88->bank.reserved_voice_mask;
        for (;;) {
            if (used_mask & mask) {
                mask <<= 1;
                voice_index++;
                if (mask & valid_mask) {
                    continue;
                }
            }
            break;
        }
        if (mask & 0xFFFFFF) {
            D_8009CD88->bank.reserved_voice_mask |= mask;
            track->voice_index = voice_index;
            track->flags |= AKAO_TRACK_FLAG_VOICE_ALLOCATED;
        }
    }

    SeqOp_SetTrack38Mask(track, voice_mask);
}

void SeqOp_SetSustainFlag(AkaoTrack *track) {
    track->flags |= AKAO_TRACK_FLAG_SUSTAIN;
}

void SeqOp_StopVoice(AkaoTrack *track, u32 arg1) {
    AkaoSequencerBank *state;
    u32 mask;

    if (track->parent_track_id == 0) {
        state = g_AkaoCurTrack;
        mask = arg1 ^ ~0xFF000000u;

        state->active_voice_mask &= mask;
        if (state->active_voice_mask == 0) {
            g_AkaoSeqLoopCounter = 0;
            state->bank_id = 0;
        }

        state = g_AkaoCurTrack;
        {
            u32 temp0;
            u32 temp1;

            temp0 = state->pending_voice_mask;
            temp1 = state->key_off_dirty_mask;
            temp0 &= mask;
            state->pending_voice_mask = temp0;
            temp0 = state->yield_voice_mask;
            temp1 &= mask;
            state->key_off_dirty_mask = temp1;
            temp1 = state->adsr_dirty_mask;
            temp0 &= mask;
            state->yield_voice_mask = temp0;
            temp0 = state->volume_dirty_mask;
            temp1 &= mask;
            state->adsr_dirty_mask = temp1;
            temp0 &= mask;
            state->volume_dirty_mask = temp0;
        }

        if ((track->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) != 0) {
            state->reserved_voice_mask &= ~(1 << track->voice_index);
        }
    } else {
        SeqOp_DeactivateVoice(track, arg1);
    }

    {
        AkaoU32 *flags = &track->flags;
        *flags = 0;
    }
    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}

void SeqOp_Noop(AkaoTrack *track, u32 mask) {
    SeqOp_StopVoice(track, mask);
}
