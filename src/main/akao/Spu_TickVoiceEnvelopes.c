#include "pe1/akao/track.h"
#include "pe1/akao/voice_state.h"

void Spu_TickVoiceEnvelopes(AkaoTrack *track, unsigned voice_mask)
{
    register unsigned old_value asm("$3");
    unsigned new_value;
    register unsigned delta asm("$4");
    register int scaled asm("$5");
    unsigned product;
    register unsigned pitch_depth asm("$2");
    register unsigned pitch_masked asm("$3");
    register int doubled_pitch asm("$2");
    register int volume_stage asm("$3");
    register int shifted_volume asm("$2");
    register int wave_sample asm("$3");
    register short *wave asm("$4");

    if (track->expression_duration) {
        old_value = track->expression_value;
        delta = track->expression_delta;
        track->expression_duration--;
        new_value = old_value + delta;
        if ((new_value & 0xFFE00000) != (old_value & 0xFFE00000))
            track->update_flags |= 3;
        track->expression_value = new_value;
    }

    if (track->key_on_delay) {
        if (--track->key_on_delay == 0) {
            D_800BCD6C ^= voice_mask;
            g_AkaoVoiceUpdateFlags |= 0x10;
            Seq_MarkTrack34MaskDirty();
        }
    }
    if (track->key_off_delay) {
        if (--track->key_off_delay == 0) {
            D_800BCD74 ^= voice_mask;
            Seq_MarkTrack3CMaskDirty();
        }
    }

    if (track->pitch_lfo_slide_duration) {
        register unsigned pitch_weight asm("$4");
        --track->pitch_lfo_slide_duration;
        track->pitch_lfo_target += track->pitch_lfo_delta;
        pitch_masked = track->pitch_lfo_target & 0x7F00;
        pitch_weight = pitch_masked >> 8;
        if (track->pitch_lfo_target & 0x8000)
            product = pitch_weight * track->pitch_base;
        else
            product = pitch_weight * (((track->pitch_base << 4) - track->pitch_base) >> 8);
        pitch_depth = product >> 7;
        asm volatile("" : "=r"(pitch_depth) : "0"(pitch_depth));
        track->pitch_lfo_depth = pitch_depth;
        if (track->pitch_lfo_phase != 1) {
            wave = (short *)track->pitch_lfo_table;
            if (wave[0] == 0 && wave[1] == 0) wave += wave[2];
            scaled = ((int)track->pitch_lfo_depth * wave[0]) >> 16;
            if (scaled != track->pitch_lfo_value) {
                track->pitch_lfo_value = scaled;
                track->update_flags |= 0x10;
                if (scaled >= 0) {
                    doubled_pitch = scaled << 1;
                    track->pitch_lfo_value = doubled_pitch;
                }
            }
        }
    }

    if (track->volume_lfo_slide_duration) {
        --track->volume_lfo_slide_duration;
        track->volume_lfo_target += track->volume_lfo_delta;
        if (track->volume_lfo_phase != 1) {
            wave = (short *)track->volume_lfo_table;
            if (wave[0] == 0 && wave[1] == 0) wave += wave[2];
            volume_stage = ((short)(track->expression_value >> 16) *
                            (track->volume_base >> 8)) >> 7;
            product = volume_stage * (track->volume_lfo_target >> 8);
            wave_sample = wave[0];
            shifted_volume = product << 9;
            scaled = shifted_volume >> 16;
            scaled = (scaled * wave_sample) >> 15;
            if (scaled != track->volume_lfo_value) {
                track->volume_lfo_value = scaled;
                track->update_flags |= 3;
            }
        }
    }

    if (track->aux_lfo_slide_duration) {
        --track->aux_lfo_slide_duration;
        track->aux_lfo_target += track->aux_lfo_delta;
        if (track->aux_lfo_phase != 1) {
            wave = (short *)track->aux_lfo_table;
            if (wave[0] == 0 && wave[1] == 0) wave += wave[2];
            scaled = ((track->aux_lfo_target >> 8) * wave[0]) >> 15;
            if (scaled != track->aux_lfo_value) {
                track->aux_lfo_value = scaled;
                track->update_flags |= 3;
            }
        }
    }

    if (track->field_7A) {
        old_value = track->voice_mask_a;
        delta = track->field_4C;
        track->field_7A--;
        new_value = old_value + delta;
        if ((new_value & 0xFFFF0000) != (old_value & 0xFFFF0000))
            track->update_flags |= 0x10;
        track->voice_mask_a = new_value;
    }
}
