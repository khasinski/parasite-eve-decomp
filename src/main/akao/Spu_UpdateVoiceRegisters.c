#include "common.h"
#include "pe1/akao/track.h"
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))

extern void *D_8009D2C8;
extern u32 D_8009D2C4;

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

static inline int compute_volume(AkaoTrack *voice, s16 *table) {
    int value = (((S16(voice, 0x46) * (voice->volume_base >> 8)) >> 7) *
                 (voice->volume_lfo_target >> 8));
    register int shifted asm("$2");
    register int sample asm("$5");
    shifted = value << 9;
    sample = shifted >> 16;
    value = sample * table[0];
    return value >> 15;
}

void Spu_UpdateVoiceRegisters(AkaoTrack *voice, u32 voice_mask) {
    register u32 old_value asm("$3");
    u32 new_value;
    int value;
    int scaled;
    s16 *table;

    if (voice->expression_duration != 0) {
        old_value = voice->expression_value;
        new_value = old_value + voice->expression_delta;
        voice->expression_duration--;
        if ((new_value & 0xFFE00000) != (old_value & 0xFFE00000)) {
            voice->update_flags |= 3;
        }
        voice->expression_value = new_value;
    }

    if (voice->pitch_slide_duration != 0) {
        voice->pitch_slide_duration--;
        voice->pitch_slide_current += (u16)voice->pitch_slide_delta;
        voice->update_flags |= 3;
    }

    if (voice->volume_duration != 0) {
        old_value = voice->volume_base;
        new_value = old_value + voice->volume_delta;
        voice->volume_duration--;
        if ((new_value & 0x7F00) != (old_value & 0x7F00)) {
            voice->update_flags |= 3;
        }
        voice->volume_base = new_value;
    }

    if (voice->panpot_duration != 0) {
        u32 signed_old;
        signed_old = voice->pan_target;
        new_value = signed_old + voice->pan_delta;
        voice->panpot_duration--;
        if ((voice->flags & 0x100) != 0 &&
            ((new_value & 0xFF00) != (signed_old & 0xFF00))) {
            voice->update_flags |= 3;
        }
        voice->pan_target = new_value;
    }

    if (voice->panpot_slide_duration != 0) {
        old_value = voice->panpot;
        new_value = old_value + voice->panpot_delta;
        voice->panpot_slide_duration--;
        if ((new_value & 0xFF00) != (old_value & 0xFF00)) {
            voice->update_flags |= 3;
        }
        voice->panpot = new_value;
    }

    if (voice->pitch_lfo_counter != 0) {
        voice->pitch_lfo_counter--;
    }

    if (voice->volume_lfo_counter != 0) {
        voice->volume_lfo_counter--;
    }

    if (voice->key_on_delay != 0) {
        voice->key_on_delay--;
        if (voice->key_on_delay == 0) {
            U32(D_8009D2C8, 0x34) ^= voice_mask;
            D_8009D2C4 |= 0x10;
            Seq_MarkTrack34MaskDirty();
        }
    }

    if (voice->key_off_delay != 0) {
        voice->key_off_delay--;
        if (voice->key_off_delay == 0) {
            U32(D_8009D2C8, 0x3C) ^= voice_mask;
            Seq_MarkTrack3CMaskDirty();
        }
    }

    if (voice->pitch_lfo_slide_duration != 0) {
        voice->pitch_lfo_slide_duration--;
        voice->pitch_lfo_target += (u16)voice->pitch_lfo_delta;
        value = (voice->pitch_lfo_target & 0x7F00) >> 8;
        if ((voice->pitch_lfo_target & 0x8000) != 0) {
            scaled = (value * voice->pitch_base) >> 7;
        } else {
            scaled = (value * (((voice->pitch_base << 4) - voice->pitch_base) >> 8)) >> 7;
        }
        voice->pitch_lfo_depth = scaled;

        if (voice->pitch_lfo_counter == 0 && voice->pitch_lfo_phase != 1) {
            register int lfo_value asm("$5");
            table = (s16 *)(u32)voice->pitch_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            lfo_value = (voice->pitch_lfo_depth * table[0]) >> 16;
            if (lfo_value != voice->pitch_lfo_value) {
                voice->pitch_lfo_value = lfo_value;
                voice->update_flags |= 0x10;
                if (lfo_value >= 0) {
                    register int doubled asm("$2");
                    doubled = lfo_value << 1;
                    voice->pitch_lfo_value = doubled;
                }
            }
        }
    }

    if (voice->volume_lfo_slide_duration != 0) {
        voice->volume_lfo_slide_duration--;
        voice->volume_lfo_target += (u16)voice->volume_lfo_delta;
        if (voice->volume_lfo_counter == 0 && voice->volume_lfo_phase != 1) {
            register int lfo_value asm("$5");
            table = (s16 *)(u32)voice->volume_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            lfo_value = compute_volume(voice, table);
            if (lfo_value != voice->volume_lfo_value) {
                voice->volume_lfo_value = lfo_value;
                voice->update_flags |= 3;
            }
        }
    }

    if (voice->aux_lfo_slide_duration != 0) {
        voice->aux_lfo_slide_duration--;
        voice->aux_lfo_target += (u16)voice->aux_lfo_delta;
        if (voice->aux_lfo_phase != 1) {
            register int lfo_value asm("$5");
            table = (s16 *)(u32)voice->aux_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            lfo_value = ((voice->aux_lfo_target >> 8) * table[0]) >> 15;
            if (lfo_value != voice->aux_lfo_value) {
                voice->aux_lfo_value = lfo_value;
                voice->update_flags |= 3;
            }
        }
    }

    if (voice->field_7A != 0) {
        old_value = voice->voice_mask_a;
        new_value = old_value + voice->field_4C;
        voice->field_7A--;
        if ((new_value & 0xFFFF0000) != (old_value & 0xFFFF0000)) {
            voice->update_flags |= 0x10;
        }
        voice->voice_mask_a = new_value;
    }
}
