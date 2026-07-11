#include "pe1/akao.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef void (*AkaoCommandHandler)(AkaoTrack *track, int voice_mask);

extern AkaoCommandHandler D_8009C8F0[];
extern AkaoCommandHandler D_8009CCF0[];
extern u16 D_8009B8DC[];
extern void *D_8009C080[];
extern unsigned int *D_8009D2C8;
extern int D_800BCD54;
extern int D_800BCD58;
extern int D_800BCD5C;
extern AkaoInstrument D_800B2900[];

int Akao_LookupSampleBankByte(AkaoTrack *track);
int Akao_LookupPitchPeriod(int instrument, int note, int detune);
void Akao_SetNotePitchBounded(AkaoTrack *track, int note);
void Seq_MarkTrack38MaskDirty(void);
void SeqOp_SetVoiceInstrument(AkaoTrack *track, AkaoInstrument *instrument, int sample_header);

static int Akao_NoteDurationIndex(int opcode) {
    return (unsigned int)opcode % 11;
}

static int Akao_NotePitchIndex(int opcode) {
    return (unsigned int)opcode / 11;
}

static void Akao_MarkTrackPending(AkaoTrack *track, int voice_mask) {
    if (track->parent_track_id == 0 && (D_8009D2C8[5] & voice_mask) && (unsigned int)track->assigned_voice_index < 0x18) {
        D_8009D2C8[6] |= voice_mask;
    }
}

static void Akao_MarkTrackStart(AkaoTrack *track, int voice_mask) {
    if (track->parent_track_id == 0) {
        D_8009D2C8[4] |= voice_mask;
        if ((D_8009D2C8[5] & voice_mask) && (unsigned int)track->assigned_voice_index < 0x18) {
            D_8009D2C8[6] |= voice_mask;
        }
    } else {
        D_800BCD54 |= voice_mask;
    }
}

static void Akao_MarkTrackPitchDirty(AkaoTrack *track, int voice_mask) {
    if (track->parent_track_id == 0) {
        D_8009D2C8[5] |= voice_mask;
    } else {
        D_800BCD58 |= voice_mask;
    }
}

static void Akao_ClearPendingNote(AkaoTrack *track) {
    track->key_on_delay = 0;
    track->pitch_lfo_value = 0;
    track->volume_lfo_value = 0;
    track->tremolo_phase &= ~AKAO_TRACK_FLAG_VOLUME_LFO;
}

static void Akao_LoadInstrumentIfNeeded(AkaoTrack *track, u8 *sample, int voice_mask) {
    int instrument_id;
    int adjusted_id;
    int sample_note;
    int sample_bank_offset;

    sample_note = sample[0];
    sample_bank_offset = (D_8009D2C8[0] & 0x100) != 0 ? 0x30 : 0;
    if (sample_note < 0x20) {
        adjusted_id = sample_note;
        if (track->note_pitch == adjusted_id) {
            return;
        }
    } else {
        adjusted_id = sample_note + sample_bank_offset;
        if (track->note_pitch == adjusted_id) {
            return;
        }
    }

    instrument_id = adjusted_id;
    track->note_pitch = instrument_id;
    SeqOp_SetVoiceInstrument(track, &D_800B2900[instrument_id], D_800B2900[instrument_id].start_address);
}

static void Akao_UpdateVoiceLfos(AkaoTrack *track, int pitch_period) {
    int depth;
    int value;

    if (track->flags & AKAO_TRACK_FLAG_PITCH_LFO) {
        depth = (track->pitch_lfo_target & 0x7F00) >> 8;
        if (track->pitch_lfo_target & 0x8000) {
            value = depth * pitch_period;
        } else {
            value = depth * (((pitch_period << 4) - pitch_period) >> 8);
        }
        track->pitch_lfo_depth = (u16)(value >> 7);
        track->pitch_lfo_counter = track->pitch_lfo_delay;
        track->pitch_lfo_phase = 1;
        track->pitch_lfo_table = D_8009C080[track->pitch_lfo_selector];
    }

    if (track->flags & AKAO_TRACK_FLAG_VOLUME_LFO) {
        track->volume_lfo_counter = track->volume_lfo_delay;
        track->volume_lfo_phase = 1;
        track->volume_lfo_table = D_8009C080[track->volume_lfo_selector];
    }

    if (track->flags & AKAO_TRACK_FLAG_AUX_LFO) {
        track->aux_lfo_phase = 1;
        track->aux_lfo_table = D_8009C080[track->aux_lfo_selector];
    }

    track->pitch_lfo_value = 0;
    track->volume_lfo_value = 0;
    track->voice_mask_a = 0;
}

void Akao_StepSampleLoader(AkaoTrack *track, int voice_mask) {
    int opcode;
    int sample_bank;
    int note;
    int note_pitch;
    int pitch_period;
    int flags;
    int value;
    int step_count;
    u8 *sample;
    AkaoCommandHandler handler;

    do {
        opcode = *track->pc++;
        if ((unsigned int)opcode < 0xA0) {
            break;
        }

        if (opcode == 0xFC) {
            opcode = *track->pc++;
            handler = D_8009CCF0[opcode];
        } else {
            if (opcode == 0xCA && (track->flags & AKAO_TRACK_FLAG_SUSTAIN)) {
                D_800BCD5C |= voice_mask;
                opcode = 0xA0;
            }
            handler = D_8009C8F0[opcode];
        }

        handler(track, voice_mask);
    } while ((unsigned int)opcode >= 0xA0 && opcode != 0xA0);

    if (opcode == 0xA0) {
        Akao_MarkTrackPending(track, voice_mask);
        return;
    }

    sample_bank = Akao_LookupSampleBankByte(track) & 0xFF;
    if (track->field_D2 != 0) {
        track->pan_duration = track->field_D2;
        track->field_56 = track->field_D2;
    }

    if (track->field_56 != 0) {
        if (sample_bank >= 0x8F || (sample_bank < 0x84 && (track->tremolo_phase & 5) == 0)) {
            track->pan_duration -= 2;
        }
    } else {
        value = D_8009B8DC[Akao_NoteDurationIndex(opcode)];
        track->field_56 = value;
        if ((unsigned int)(sample_bank - 0x84) >= 0xB && (track->tremolo_phase & 5) == 0) {
            value -= 2;
        }
        track->pan_duration = value;
    }

    track->field_D0 = track->field_56;
    track->update_flags |= 0x4000;
    if (sample_bank < 0x8F) {
        track->flags &= ~0x40;
    } else {
        track->flags |= 0x40;
    }

    if ((unsigned int)opcode >= 0x8F) {
        Akao_MarkTrackPending(track, voice_mask);
        Akao_ClearPendingNote(track);
        return;
    }

    if ((unsigned int)opcode >= 0x84) {
        track->tremolo_phase = (track->tremolo_phase & ~AKAO_TRACK_FLAG_VOLUME_LFO) | ((track->tremolo_phase & 1) << 1);
        if (track->vibrato_delta != 0) {
            note_pitch = track->field_E2 + track->vibrato_delta;
            track->field_E2 = note_pitch;
            pitch_period = Akao_LookupPitchPeriod(track->note_pitch, note_pitch + track->expression, track->detune);
            value = ((pitch_period << 16) - ((track->pitch_base << 16) + track->voice_mask_a)) / track->vibrato_duration;
            track->vibrato_delta = 0;
            track->field_7A = track->vibrato_duration;
            track->field_4C = value;
        }
        track->tremolo_counter = track->field_E2;
        track->tremolo_delta = track->expression;
        return;
    }

    note = Akao_NotePitchIndex(opcode);
    flags = track->flags;
    if (flags & AKAO_TRACK_FLAG_BRANCH_ACTIVE) {
        D_8009D2C8[4] |= voice_mask;
        if ((D_8009D2C8[5] & voice_mask) && (unsigned int)track->assigned_voice_index < 0x18) {
            D_8009D2C8[6] |= voice_mask;
        }

        sample = (u8 *)track->branch_target + (((unsigned int)note / 12) * 6);
        Akao_LoadInstrumentIfNeeded(track, sample, voice_mask);
        pitch_period = Akao_LookupPitchPeriod(track->note_pitch, sample[1], track->detune);
        track->expression_value = (track->volume * (sample[2] | (sample[3] << 8))) << 2;
        track->panpot = (u16)(((sample[4] + 0x40) & 0xFF) << 8);
        if (sample[5] != 0) {
            D_8009D2C8[0xE] |= voice_mask;
        } else {
            D_8009D2C8[0xE] &= ~voice_mask;
        }
        Seq_MarkTrack38MaskDirty();
    } else {
        note += track->panpot_step * 12;
        if ((track->tremolo_phase & AKAO_TRACK_FLAG_VOLUME_LFO) == 0) {
            if (track->parent_track_id == 0) {
                if (flags & AKAO_TRACK_FLAG_PENDING_NOTE_PITCH) {
                    Akao_SetNotePitchBounded(track, note);
                }
                Akao_MarkTrackStart(track, voice_mask);
            } else {
                D_800BCD54 |= voice_mask;
            }
            track->field_7A = 0;
        }

        if (track->key_on_delay != 0 && track->tremolo_counter != 0) {
            track->vibrato_duration = track->key_on_delay;
            note = track->tremolo_counter + track->tremolo_delta;
            value = track->expression + note - track->tremolo_counter - track->tremolo_delta;
            track->vibrato_delta = value;
            track->field_E2 = track->tremolo_counter - (track->expression - track->tremolo_delta);
        } else {
            track->field_E2 = note;
            note += track->expression;
        }
        pitch_period = Akao_LookupPitchPeriod(track->note_pitch, note, track->detune);
    }

    track->pitch_base = pitch_period;
    Akao_MarkTrackPitchDirty(track, voice_mask);
    track->update_flags |= 0x13;
    Akao_UpdateVoiceLfos(track, pitch_period);

    track->tremolo_phase = (track->tremolo_phase & ~AKAO_TRACK_FLAG_VOLUME_LFO) | ((track->tremolo_phase & 1) << 1);
    if (track->vibrato_delta != 0) {
        note_pitch = track->field_E2 + track->vibrato_delta;
        track->field_E2 = note_pitch;
        pitch_period = Akao_LookupPitchPeriod(track->note_pitch, note_pitch + track->expression, track->detune);
        step_count = track->vibrato_duration;
        value = ((pitch_period << 16) - ((track->pitch_base << 16) + track->voice_mask_a)) / step_count;
        track->vibrato_delta = 0;
        track->field_7A = step_count;
        track->field_4C = value;
    }

    track->tremolo_counter = track->field_E2;
    track->tremolo_delta = track->expression;
}
