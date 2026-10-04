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
