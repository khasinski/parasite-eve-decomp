#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/track.h"
#include "pe1/akao/voice_state.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/psyq_spu_internal.h"

extern volatile u16 D_1F801D88;
extern volatile u16 D_1F801D8A;

void Spu_WriteKeyOn(u32 value) {
    D_1F801D88 = value;
    value >>= 16;
    D_1F801D8A = value;
}

extern volatile u16 D_1F801D8C;
extern volatile u16 D_1F801D8E;

void Spu_WriteKeyOff(u32 value) {
    D_1F801D8C = value;
    value >>= 16;
    D_1F801D8E = value;
}

extern volatile u16 D_1F801D98;
extern volatile u16 D_1F801D9A;

void Spu_WriteReverbEnable(u32 value) {
    D_1F801D98 = value;
    value >>= 16;
    D_1F801D9A = value;
}

extern volatile u16 D_1F801D94;
extern volatile u16 D_1F801D96;

void Spu_WriteNoiseEnable(u32 value) {
    D_1F801D94 = value;
    value >>= 16;
    D_1F801D96 = value;
}

extern volatile u16 D_1F801D90;
extern volatile u16 D_1F801D92;

void Spu_WriteFmEnable(u32 value) {
    D_1F801D90 = value;
    value >>= 16;
    D_1F801D92 = value;
}

void AkaoSpuVoice_SetVolume(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 mask;

    ptr = (u16 *)(0x1F801C00 + (index * 0x10));
    mask = 0x7FFF;
    ptr[0] = left & mask;
    ptr++;
    *ptr = right & 0x7FFF;
}

void AkaoSpuVoice_SetPitch(u32 index, u32 value) {
    u16 *ptr;

    ptr = (u16 *)(0x1F800000 + (index * 0x10));
    ptr[0x1C04 / 2] = value;
}

void AkaoSpuVoice_SetStartAddress(u32 index, u32 value) {
    u16 *ptr;

    ptr = (u16 *)(0x1F800000 + (index * 0x10));
    ptr[0x1C06 / 2] = value >> 3;
}

void AkaoSpuVoice_SetRepeatAddress(u32 index, u32 value) {
    u16 *ptr;

    ptr = (u16 *)(0x1F800000 + (index * 0x10));
    ptr[0x1C0E / 2] = value >> 3;
}

void AkaoSpuVoice_SetAdsrAttack(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    right = (right >> 2) << 15;
    left <<= 8;
    current = *(u8 *)ptr;
    value = right | left;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrDecayRate(u32 index, u32 value) {
    u16 *ptr;
    u32 current;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    current = *ptr;
    value <<= 4;
    current &= 0xFF0F;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrSustainLevel(u32 index, u32 value) {
    u16 *ptr;
    u32 current;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    current = *ptr;
    current &= 0xFFF0;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrSustainRate(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C0A + (index * 0x10));
    right = (right >> 1) << 14;
    left <<= 6;
    current = *ptr;
    value = right | left;
    current &= 0x3F;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrReleaseRate(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C0A + (index * 0x10));
    right = (right >> 2) << 5;
    current = *ptr;
    value = right | left;
    current &= 0xFFC0;
    *ptr = current | value;
}

/* Old-style definition: the stereo and note callers pass the track flags
   (and a mask) after the two parameters read here, so this unit keeps the
   unprototyped call ABI that voice_masks.h declares. */
void Akao_WriteVoiceParam(voice_index, params)
    int voice_index;
    AkaoVoiceParams *params;
{
    unsigned int flags;
    unsigned int cur;

    flags = params->flags;
    if (flags != 0) {
        if (flags & AKAO_VOICE_PARAM_PITCH) {
            AkaoSpuVoice_SetPitch(voice_index, params->pitch);
            params->flags &= ~AKAO_VOICE_PARAM_PITCH;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_VOLUME) {
            AkaoSpuVoice_SetVolume(voice_index, params->volume_left, params->volume_right);
            params->flags &= ~AKAO_VOICE_PARAM_VOLUME;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_START_ADDR) {
            AkaoSpuVoice_SetStartAddress(voice_index, params->start_address);
            params->flags &= ~AKAO_VOICE_PARAM_START_ADDR;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_LOOP_ADDR) {
            AkaoSpuVoice_SetRepeatAddress(voice_index, params->loop_address);
            cur = params->flags & ~AKAO_VOICE_PARAM_LOOP_ADDR;
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_SUSTAIN) {
            AkaoSpuVoice_SetAdsrSustainRate(voice_index, params->adsr_sustain_rate, params->adsr_sustain_mode);
            params->flags &= ~AKAO_VOICE_PARAM_ADSR_SUSTAIN;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_ATTACK) {
            AkaoSpuVoice_SetAdsrAttack(voice_index, params->adsr_attack_rate, params->adsr_attack_mode);
            params->flags &= ~AKAO_VOICE_PARAM_ADSR_ATTACK;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_RELEASE) {
            AkaoSpuVoice_SetAdsrReleaseRate(voice_index, params->adsr_release_rate, params->adsr_release_mode);
            params->flags &= ~AKAO_VOICE_PARAM_ADSR_RELEASE;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_DECAY_SUSTAIN) {
            AkaoSpuVoice_SetAdsrDecayRate(voice_index, params->adsr_decay_rate);
            AkaoSpuVoice_SetAdsrSustainLevel(voice_index, params->adsr_sustain_level);
        }
        params->flags = 0;
    }
}

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
    u32 new_value;
    int value;
    int scaled;
    s16 *table;

    if (voice->expression_duration != 0) {
        u32 old_value;
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
        u32 old_value;
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
        u32 old_value;
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
            table = (s16 *)voice->pitch_lfo_table;
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
            table = (s16 *)voice->volume_lfo_table;
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
            table = (s16 *)voice->aux_lfo_table;
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

    if (voice->pitch_slide_steps != 0) {
        u32 old_value;
        old_value = voice->voice_mask_a;
        new_value = old_value + voice->pitch_slide_step;
        voice->pitch_slide_steps--;
        if ((new_value & 0xFFFF0000) != (old_value & 0xFFFF0000)) {
            voice->update_flags |= 0x10;
        }
        voice->voice_mask_a = new_value;
    }
}

void Spu_TickVoiceEnvelopes(AkaoTrack *track, unsigned voice_mask)
{
    register unsigned old_value asm("$3");
    unsigned new_value;
    unsigned delta;
    register int scaled asm("$5");
    unsigned product;
    unsigned pitch_depth;
    unsigned pitch_masked;
    register int doubled_pitch asm("$2");
    int volume_stage;
    register int shifted_volume asm("$2");
    int wave_sample;
    short *wave;

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
        unsigned pitch_weight;
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

    if (track->pitch_slide_steps) {
        old_value = track->voice_mask_a;
        delta = track->pitch_slide_step;
        track->pitch_slide_steps--;
        new_value = old_value + delta;
        if ((new_value & 0xFFFF0000) != (old_value & 0xFFFF0000))
            track->update_flags |= 0x10;
        track->voice_mask_a = new_value;
    }
}

#define U16(base, off) (*(u16 *)((char *)(base) + (off)))
#define ADVANCE_ENV_PTR(voice, off, timer, reset) do { \
    u16 reset_value = U16((voice), (reset)); \
    table = (s16 *)U32((voice), (off)); \
    U16((voice), (timer)) = reset_value; \
    if (table[0] == 0 && table[1] == 0) { \
        register s16 *next asm("$2") = table + table[2]; \
        U32((voice), (off)) = (u32)next; \
    } \
    table = (s16 *)U32((voice), (off)); \
    U32((voice), (off)) = (u32)(table + 1); \
    sample = *table; \
} while (0)
#define MIX_PAN_DISPATCH(voice, volume, pan_value, mask_value, label, mode_value, one_value) do { \
    int left_value; \
    if ((mode_value) == (one_value)) goto label##_case1; \
    if ((mode_value) == 4) goto label##_case4; \
    goto label##_default; \
    label##_case1: \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        S16((voice), 0x11A) = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        goto label##_done; \
    label##_case4: { \
        register int right asm("$3"); \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        right = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        S16((voice), 0x11A) = right; \
        if (((mask_value) & 0x00AAAAAA) != 0) { \
            register int flip asm("$2") = ~right; \
            S16((voice), 0x11A) = flip; \
            do { } while (0); \
        } else { \
            left_value = ~U16((voice), 0x118); \
            goto label##_store_left; \
        } \
        goto label##_done; \
    } \
    label##_default: { \
        left_value = (u32)((volume) * D_8009B9F8[0]) >> 15; \
        S16((voice), 0x11A) = left_value; \
        label##_store_left: \
        S16((voice), 0x118) = left_value; \
        goto label##_done; \
    } \
    label##_done:; \
} while (0)
#define MIX_PAN_MODE(voice, volume, pan_value, mask_value, label, mode_value) do { \
    int left_value; \
    switch (mode_value) { \
    case 1: \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        S16((voice), 0x11A) = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        break; \
    case 4: { \
        register int right asm("$3"); \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        right = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        S16((voice), 0x11A) = right; \
        if (((mask_value) & 0x00AAAAAA) != 0) { \
            register int flip asm("$2") = ~right; \
            S16((voice), 0x11A) = flip; \
            do { } while (0); \
        } else { \
            left_value = ~U16((voice), 0x118); \
            goto label##_store_left; \
        } \
        break; \
    } \
    default: { \
        left_value = (u32)((volume) * D_8009B9F8[0]) >> 15; \
        S16((voice), 0x11A) = left_value; \
        label##_store_left: \
        S16((voice), 0x118) = left_value; \
        break; \
    } \
    } \
} while (0)
#define SCALE_GLOBAL_DEPTH(value, depth) do { \
if (depth != 0) { \
    do { \
        if (depth < 0x80) { \
            (value) += ((value) * depth) >> 7; \
            break; \
        } \
        (value) = ((value) * depth) >> 8; \
    } while (0); \
} \
} while (0)

extern int D_8009D2C0;
extern unsigned char D_8009D2CE;
extern s16 D_8009B8F8[];
extern s16 D_8009B9F8[];
extern char D_800B8AC0[];

void Akao_SetVoiceKeyOff(AkaoTrack *cursor, u32 voice_mask, int index) {
    register void *voice = cursor;
    u32 flags;
    register int base_volume asm("$4");
    int value;
    int sample;
    int pan;
    void *target_voice;
    register s16 *table asm("$3");

    base_volume = (S16(voice, 0x46) * (U16(voice, 0x6C) >> 8)) >> 7;
    flags = U32(voice, 0x38);

    if ((flags & 1) != 0 && U16(voice, 0x8A) == 0) {
        U16(voice, 0x8E)--;
        if (U16(voice, 0x8E) == 0) {
            ADVANCE_ENV_PTR(voice, 0x1C, 0x8E, 0x8C);
            { register int final_value asm("$7") = (U16(voice, 0x92) * sample) >> 16;
              if (final_value != S16(voice, 0xE8)) {
                  U16(voice, 0xE8) = final_value;
                  U32(voice, 0xF4) |= 0x10;
                  if (final_value >= 0) {
                      register int doubled asm("$2") = final_value << 1;
                      U16(voice, 0xE8) = doubled;
                  }
              }
            }
        }
    }

    if ((flags & 2) != 0 && U16(voice, 0x9E) == 0) {
        U16(voice, 0xA2)--;
        if (U16(voice, 0xA2) == 0) {
            { u16 reset_value = U16(voice, 0xA0);
              table = (s16 *)U32(voice, 0x20);
              U16(voice, 0xA2) = reset_value;
              if (table[0] == 0 && table[1] == 0) {
                  { register s16 *next asm("$2") = table + table[2]; U32(voice, 0x20) = (u32)next; }
              }
            }
            value = base_volume * (U16(voice, 0xA6) >> 8);
            table = (s16 *)U32(voice, 0x20);
            U32(voice, 0x20) = (u32)(table + 1);
            sample = *table;
            { register int shifted asm("$2") = value << 9; register int upper asm("$7") = shifted >> 16;
              register int final_value = (upper * sample) >> 15;
              if (final_value != S16(voice, 0xEA)) {
                  U16(voice, 0xEA) = final_value;
                  U32(voice, 0xF4) |= 3;
              }
            }
        }
    }

    if ((flags & 4) != 0) {
        U16(voice, 0xB0)--;
        if (U16(voice, 0xB0) == 0) {
            { u16 reset_value = U16(voice, 0xAE);
              table = (s16 *)U32(voice, 0x24);
              U16(voice, 0xB0) = reset_value;
              if (table[0] == 0 && table[1] == 0) {
                  register s16 *next asm("$2") = table + table[2];
                  U32(voice, 0x24) = (u32)next;
              }
            }
            table = (s16 *)U32(voice, 0x24);
            U32(voice, 0x24) = (u32)(table + 1);
            { register int scale = U16(voice, 0xB4) >> 8;
              sample = *table;
              { register int final_value = (scale * sample) >> 15;
                if (final_value != S16(voice, 0xEC)) {
                    U16(voice, 0xEC) = final_value;
                    U32(voice, 0xF4) |= 3;
                }
              }
            }
        }
    }

    if ((flags & 0x20) != 0) {
        base_volume = (((int)(U16(voice, -0x10) << 17) >> 16) *
                       (U16(voice, 0x6C) >> 8)) >> 7;
        U32(voice, 0xF4) |= 3;
    }

    if ((U32(voice, 0xF4) & 3) != 0) {
        { int product; int mode; register int one;
          base_volume += S16(voice, 0xEA);
          product = base_volume * (U16(D_8009D2C8, 0x4A) & 0x7F);
          { register int raw_pan = U16(voice, 0x76); register int bias = S16(voice, 0xEC); register int shifted asm("$2") = raw_pan >> 8; int sum = shifted + bias; pan = sum & 0xFF; }
          one = 1;
          mode = D_8009D2C0;

          base_volume = product >> 7;
          MIX_PAN_DISPATCH(voice, base_volume, pan, voice_mask, primary, mode, one);
        }

        if ((flags & 0x800) != 0) {
            target_voice = D_800B8AC0 + U16(voice, 0x5C) * 0x11C;
            S16(target_voice, 0x118) = U16(voice, 0x118);
            S16(target_voice, 0x11A) = U16(voice, 0x11A);

            { register int raw asm("$2") = U16(voice, 0x5E);
              int mode = D_8009D2C0;
              base_volume = raw >> 1;
              MIX_PAN_MODE(voice, base_volume, pan, voice_mask, secondary, mode);
            }
            U16(target_voice, 0x118) -= U16(voice, 0x118);
            U16(target_voice, 0x11A) -= U16(voice, 0x11A);
        }
    }

    if ((flags & 0x10) != 0) {
        register int depth asm("$7") = D_8009D2CE;
        int pitch = U16(voice, -0x10) + S16(voice, 0xE8) + S16(voice, 0x36);
        SCALE_GLOBAL_DEPTH(pitch, depth);
        U16(voice, 0x10C) = pitch & 0x3FFF;
        U32(voice, 0xF4) |= 0x10;
    } else if ((U32(voice, 0xF4) & 0x10) != 0) {
        register int depth asm("$7") = D_8009D2CE;
        register int pitch = U32(voice, 0x30) + S16(voice, 0xE8) + S16(voice, 0x36);
        SCALE_GLOBAL_DEPTH(pitch, depth);
        { int masked = pitch & 0x3FFF; U16(voice, 0x10C) = masked; }
    }
}

#undef ADVANCE_ENV_PTR
#define ADVANCE_ENV_PTR(voice, off)                                  \
    do {                                                            \
        if (table[0] == 0 && table[1] == 0) {                        \
            register s16 *next = table + table[2];        \
            U32((voice), (off)) = (u32)next;                         \
        }                                                           \
        table = (s16 *)U32((voice), (off));                          \
        U32((voice), (off)) = (u32)(table + 1);                      \
        { int raw = *table;                      \
           sample = raw; }             \
    } while (0)
void Akao_SetVoiceKeyOn(AkaoTrack *voice, u32 voice_mask) {
    register u32 flags;
    register int base_volume;
    u32 later_flags;
    int value;
    int sample;
    int pan;
    register int mode asm("$4");
    s16 *table;

    base_volume = (S16(voice, 0x46) * (U16(voice, 0x6C) >> 8)) >> 7;
    flags = U32(voice, 0x38);

    if ((flags & 1) != 0) {
        U16(voice, 0x8E)--;
        if (U16(voice, 0x8E) == 0) {
            register int value;
            table = (s16 *)U32(voice, 0x1C);
            U16(voice, 0x8E) = U16(voice, 0x8C);
            ADVANCE_ENV_PTR(voice, 0x1C);
            value = (U16(voice, 0x92) * sample) >> 16;
            if (value != S16(voice, 0xE8)) {
                U16(voice, 0xE8) = value;
                U32(voice, 0xF4) |= 0x10;
                if (value >= 0) {
                    int doubled = value << 1;
                    U16(voice, 0xE8) = doubled;
                }
            }
        }
    }

    if ((flags & 2) != 0) {
        U16(voice, 0xA2)--;
        if (U16(voice, 0xA2) == 0) {
            int volume_product;
            table = (s16 *)U32(voice, 0x20);
            U16(voice, 0xA2) = U16(voice, 0xA0);
            if (table[0] == 0 && table[1] == 0) {
                register s16 *next = table + table[2];
                U32(voice, 0x20) = (u32)next;
            }

            volume_product = base_volume * (U16(voice, 0xA6) >> 8);
            table = (s16 *)U32(voice, 0x20);
            U32(voice, 0x20) = (u32)(table + 1);
            sample = *table;
            { register int shifted asm("$2") = volume_product << 9;
              register int upper asm("$3") = shifted >> 16;
              value = (upper * sample) >> 15; }
            { register int final_value asm("$3") = value;
              if (final_value != S16(voice, 0xEA)) {
                  U16(voice, 0xEA) = final_value;
                  U32(voice, 0xF4) |= 3;
              }
            }
        }
    }

    later_flags = flags;
    if ((later_flags & 4) != 0) {
        U16(voice, 0xB0)--;
        if (U16(voice, 0xB0) == 0) {
            register int value;
            int scale;
            table = (s16 *)U32(voice, 0x24);
            U16(voice, 0xB0) = U16(voice, 0xAE);
            if (table[0] == 0 && table[1] == 0) {
                register s16 *next = table + table[2];
                U32(voice, 0x24) = (u32)next;
            }

            table = (s16 *)U32(voice, 0x24);
            U32(voice, 0x24) = (u32)(table + 1);
            scale = U16(voice, 0xB4) >> 8;
            { int raw = *table;  sample = raw; }
            value = (scale * sample) >> 15;
            if (value != S16(voice, 0xEC)) {
                U16(voice, 0xEC) = value;
                U32(voice, 0xF4) |= 3;
            }
        }
    }

    if ((later_flags & 0x20) != 0) {
        base_volume = (((int)(U16(voice, -0x10) << 17) >> 16) *
                       (U16(voice, 0x6C) >> 8)) >> 7;
        U32(voice, 0xF4) |= 3;
    }

    if ((U32(voice, 0xF4) & 3) != 0) {
        base_volume += S16(voice, 0xEA);
        if ((U32(voice, 0x2C) & 0x02000000) != 0) {
            pan = 0x80;
        } else {
            base_volume = (base_volume * (s8)(U16(voice, 0xD8) >> 8)) >> 7;
            pan = ((U16(voice, 0x76) >> 8) + S16(voice, 0xEC)) & 0xFF;
        }

        mode = D_8009D2C0;
        switch (mode) {
        case 1:
            S16(voice, 0x118) = (base_volume * D_8009B8F8[pan]) >> 15;
            S16(voice, 0x11A) = (base_volume * D_8009B8F8[pan ^ 0xFF]) >> 15;
            break;
        case 4: {
            int right;
            S16(voice, 0x118) = (base_volume * D_8009B8F8[pan]) >> 15;
            right = (base_volume * D_8009B8F8[pan ^ 0xFF]) >> 15;
            S16(voice, 0x11A) = right;
            if ((voice_mask & 0x00AAAAAA) != 0) {
                int flip = ~right;
                S16(voice, 0x11A) = flip;
            } else {
                S16(voice, 0x118) = ~U16(voice, 0x118);
            }
            break;
        }
        default:
            value = (S16(voice, 0x11A) =
                (u32)(base_volume * D_8009B9F8[0]) >> 15);
            S16(voice, 0x118) = value;
            break;
        }
    }

    if ((later_flags & 0x10) != 0) {
        value = U16(voice, -0x10) + S16(voice, 0xE8) + S16(voice, 0x36);
        if ((U32(voice, 0x2C) & 0x02000000) == 0) {
            int depth = *(u8 *)((char *)voice + 0x3D);
            if (depth != 0) {
                if (depth < 0x80) {
                    value += (value * depth) >> 7;
                } else {
                    value = (value * depth) >> 8;
                }
            }
        }
        U16(voice, 0x10C) = value & 0x3FFF;
        U32(voice, 0xF4) |= 0x10;
    } else if ((U32(voice, 0xF4) & 0x10) != 0) {
        value = U32(voice, 0x30) + S16(voice, 0xE8) + S16(voice, 0x36);
        if ((U32(voice, 0x2C) & 0x02000000) == 0) {
            int depth = *(u8 *)((char *)voice + 0x3D);
            if (depth != 0) {
                if (depth < 0x80) {
                    value += (value * depth) >> 7;
                } else {
                    value = (value * depth) >> 8;
                }
            }
        }
        U16(voice, 0x10C) = value & 0x3FFF;
    }
}

extern AkaoTrack g_AkaoVoiceStateTable[];

void Spu_CopyVoiceToStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    int left;
    int right;
    int pan;
    int new_var;
    register int inverse_pan;
    AkaoTrack *stereo;

    left = track->volume_left;
    inverse_pan = 0x7F;
    pan = (unsigned short)track->pan_target;
    pan = ((int)(pan << 16)) >> 24;
    inverse_pan -= pan;
    track->volume_left = ((unsigned int)(left * inverse_pan)) >> 8;
    stereo = &g_AkaoVoiceStateTable[stereo_voice_index];
    new_var = inverse_pan;
    stereo->volume_left = (left * ((short)track->pan_target)) >> 16;
    right = (left = track->volume_right);
    track->volume_right = ((unsigned int)(right * new_var)) >> 8;
    stereo->volume_right = (right * ((short)track->pan_target)) >> 16;
    stereo->pitch = track->pitch;
    stereo->update_flags |= (*track).update_flags;
    Akao_WriteVoiceParam(track->assigned_voice_index, (AkaoVoiceParams *)(&track->assigned_voice_index), track->flags);
    Akao_WriteVoiceParam(stereo_voice_index, (AkaoVoiceParams *)(&stereo->assigned_voice_index), track->flags);
}

void Spu_RestoreVoiceFromStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    AkaoTrack *track_reg;
    int index_reg;
    AkaoVoiceParams *params;
    register int mask asm("$7");
    int idx_arg;
    int flags_arg;
    AkaoVoiceParams *call_params;
    register u32 left asm("$3");
    u32 right;
    register u32 flags asm("$3");
    AkaoTrack *stereo;

    track_reg = track;
    index_reg = stereo_voice_index;
    params = &AKAO_TRACK_VOICE(track_reg);
    Akao_WriteVoiceParam(track_reg->assigned_voice_index, params, track_reg->flags);
    mask = 0x1FF93;
    idx_arg = index_reg;
    asm volatile("" : "=r"(idx_arg) : "0"(idx_arg));
    stereo = &g_AkaoVoiceStateTable[idx_arg];
    flags_arg = track_reg->flags;
    asm volatile("" : "=r"(flags_arg) : "0"(flags_arg));
    left = (unsigned short)stereo->volume_left;
    call_params = params;
    track_reg->volume_left = left;
    flags = track_reg->update_flags;
    right = (unsigned short)stereo->volume_right;
    flags = flags | mask;
    track_reg->update_flags = flags;
    track_reg->volume_right = right;
    Akao_WriteVoiceParam(idx_arg, call_params, flags_arg, mask);
}

void Akao_StepVoiceNote(AkaoTrack *track, unsigned mask, unsigned direct, unsigned *output) {
    unsigned bit = 1;
    int index = 0;
    unsigned requested = mask & g_AkaoCurTrack->key_on_request_mask;
    do {
        if (mask & bit) {
            Akao_SetVoiceKeyOff(track, bit, index);
            if (track->update_flags) {
                if (requested & bit) {
                    if (direct & bit) {
                        *output |= 1 << index;
                        track->assigned_voice_index = index;
                        track->update_flags |= 0x1ff93;
                    } else {
                        unsigned voice = 0;
                        AkaoVoiceEnvelopeSlot *slot = g_AkaoVoiceEnvelopeTable;
                        do {
                            if (!slot->level) {
                                track->update_flags |= 0x1ff93;
                                *output |= 1 << voice;
                                track->assigned_voice_index = voice;
                                *(short *)&slot->level = 0x7fff;
                                g_AkaoVoiceUpdateFlags |= 0x100;
                                voice = 24;
                            } else {
                                voice++;
                                slot++;
                                if (voice == 24) {
                                    track->assigned_voice_index = voice;
                                    g_AkaoCurTrack->status_flags |= 1;
                                }
                            }
                        } while (voice < 24);
                    }
                }
                if (g_AkaoVoicePortamentoResetMask & bit) {
                    track->volume_right = 0;
                    track->volume_left = 0;
                }
                if ((unsigned)track->assigned_voice_index < 24)
                    Akao_WriteVoiceParam(track->assigned_voice_index, (AkaoVoiceParams *)&track->assigned_voice_index, track->flags);
            }
            mask &= ~bit;
        }
        bit <<= 1;
        track++;
        index++;
    } while (mask);
}

/* The tracked voice ID is AkaoTrack::assigned_voice_index (+0xF0); retain
 * the original word-stride loop while exposing the shared track type in the API. */
void Akao_RemoveVoice(AkaoTrack *voices, int voiceIndex) {
    int *assignedVoiceIndex = (int *)voices;
    int i = 0;
    int unusedVoice = 0x18;

    assignedVoiceIndex += 0x3C;
    do {
        if (voiceIndex == *assignedVoiceIndex) {
            *assignedVoiceIndex = unusedVoice;
        }
        i++;
        assignedVoiceIndex += 0x47;
    } while ((unsigned int)i < 0x18);
}

void Akao_UpdateVoiceEnvelopes(s32 protectedMask) {
    AkaoVoiceEnvelopeSlot *envelope;
    s32 combinedMask;
    u32 voiceIndex;
    u32 mask;
    AkaoTrack *voiceBase;

    voiceIndex = 0;
    mask = 1;
    voiceBase = &g_AkaoVoiceStateTable[0];
    envelope = &g_AkaoVoiceEnvelopeTable[0];
    combinedMask = (((*((s32 *) (((u8 *) g_AkaoCurTrack) + 4))) & (*((s32 *) (((u8 *) g_AkaoCurTrack) + 0xC)))) | ((*((s32 *) (((u8 *) g_AkaoCurTrack) + 0x6C))) & (*((s32 *) (((u8 *) g_AkaoCurTrack) + 0x74))))) | protectedMask;
    do {
        if (combinedMask & (mask << voiceIndex)) {
            register s32 level = 0x7FFF;
            envelope->level = level;
        } else {
            SpuGetVoiceEnvelope(voiceIndex, (unsigned short *)envelope);
            if (envelope->level == 0) {
                if (envelope) {
                    Akao_RemoveVoice(voiceBase, voiceIndex);
                    Akao_RemoveVoice(voiceBase + AKAO_VOICE_COUNT, voiceIndex);
                } else {
                    Akao_RemoveVoice(voiceBase, voiceIndex);
                    Akao_RemoveVoice(voiceBase + AKAO_VOICE_COUNT, voiceIndex);
                }
            }
        }
        voiceIndex += 1;
        envelope++;
    } while (voiceIndex < 0x18U);
}

extern unsigned D_800BCD58, g_AkaoVoiceMaskScratch;
extern unsigned short g_AkaoTrack5ATransposeValue;
extern AkaoTrack g_AkaoVoiceChannelTable[];
void SpuSetReverbModeDepth(short, short);
void Spu_WriteReverbEnable(unsigned);
void Spu_WriteNoiseEnable(unsigned);
void Spu_WriteFmEnable(unsigned);
void Spu_WriteKeyOn(unsigned);
void Akao_ProcessVoiceQueue(void)
{
  unsigned *active = &g_SpuActiveVoiceMask;
  unsigned excluded;
  unsigned secondary;
  unsigned secondary_new;
  unsigned int allocated_primary;
  unsigned primary;
  unsigned primary_new;
  unsigned result = 0;

  register unsigned bit asm("$18");
  unsigned flags;
  AkaoSequencerBank *bank;
  AkaoTrack *track;
  excluded = (*active) | g_SpuStoppedVoiceMask;
  if ((g_AkaoCurTrack[0].active_voice_mask & g_AkaoCurTrack[0].key_on_request_mask) | (g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_on_request_mask))
  {
    Akao_UpdateVoiceEnvelopes(excluded);
  }
  secondary = (g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].allocated_voice_mask) & (~(g_AkaoCurTrack[1].field_0C & excluded));
  secondary_new = (secondary & g_AkaoCurTrack[1].field_0C) & (~excluded);
  if (secondary & g_AkaoCurTrack[1].pending_voice_mask)
  {
    g_AkaoCurTrack++;
    Akao_StepVoiceNote(g_AkaoVoiceStateTable2, secondary & g_AkaoCurTrack->pending_voice_mask, secondary_new, &result);
    bank = g_AkaoCurTrack;
    secondary &= ~bank->pending_voice_mask;
    g_AkaoCurTrack--;
    bank->key_on_request_mask &= ~bank->pending_voice_mask;
  }
  allocated_primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->allocated_voice_mask;
  primary_new = secondary_new | excluded;
  primary = allocated_primary & (~(g_AkaoCurTrack->field_0C & primary_new));
  excluded = (primary & g_AkaoCurTrack->field_0C) & (~primary_new);
  if (primary & g_AkaoCurTrack->pending_voice_mask)
  {
    Akao_StepVoiceNote(g_AkaoVoiceStateTable, primary & g_AkaoCurTrack->pending_voice_mask, excluded, &result);
    bank = g_AkaoCurTrack;
    primary &= ~bank->pending_voice_mask;
    /* Retail reloads the pending mask for this independent flag update. */
    bank->key_on_request_mask &= ~*(volatile unsigned *)&bank->pending_voice_mask;
  }
  asm("" : : "r"(secondary));
  if (secondary)
  {
    g_AkaoCurTrack++;
    Akao_StepVoiceNote(g_AkaoVoiceStateTable2, secondary, secondary_new & (~excluded), &result);
    g_AkaoCurTrack->key_on_request_mask = 0;
    g_AkaoCurTrack--;
  }
  if (primary)
  {
    Akao_StepVoiceNote(g_AkaoVoiceStateTable, primary, excluded, &result);
    g_AkaoCurTrack->key_on_request_mask = 0;
  }
  primary = (*active) & D_800BCD58;
  if (primary)
  {
    bit = 0x1000;
    track = g_AkaoVoiceChannelTable;
    result |= g_AkaoVoiceMaskScratch;
    while (primary)
    {
      if (primary & bit)
      {
        Akao_SetVoiceKeyOn(track, bit);
        if (track->update_flags)
        {
          Akao_WriteVoiceParam(track->assigned_voice_index, &track->assigned_voice_index, track->flags);
        }
        secondary_new = bit;
        primary &= ~secondary_new;
      }
      bit <<= 1;
      track++;
    }

    g_AkaoVoiceMaskScratch = 0;
  }
  primary = g_AkaoVoiceUpdateFlags;
  if (primary & 0x80)
  {
    short volume = ((short *) g_AkaoCurTrack->field_40)[1];
    SpuSetReverbModeDepth(volume, volume);
    g_AkaoVoiceUpdateFlags &= ~0x80;
  }
  if (primary & 0x10)
  {
    unsigned short clock;
    if (g_SpuActiveVoiceMask)
    {
      clock = g_AkaoTrack5ATransposeValue;
    }
    else
    {
      clock = g_AkaoCurTrack->field_5A;
    }
    SpuSetNoiseClock(clock);
    g_AkaoVoiceUpdateFlags &= ~0x10;
  }
  if (0x100 & primary)
  {
    Akao_SetVoiceAdsr();
    Akao_SetVoiceVolume();
    Akao_SetVoiceStartAddr();
    Spu_WriteReverbEnable(D_800C0DD0);
    Spu_WriteNoiseEnable(D_800C0DD4);
    Spu_WriteFmEnable(D_800C0DD8);
    g_AkaoVoiceUpdateFlags &= ~0x100;
  }
  if (result)
  {
    Spu_WriteKeyOn(result);
  }
}

void Spu_VoiceMaskCompose(AkaoTrack *track, int *mask_out, int mask, int mask_keep) {
    int bit;
    int idx;

    bit = 1;
    do {
        if ((mask & bit) != 0) {
            idx = track->assigned_voice_index;
            if ((unsigned int) idx < 0x18) {
                *mask_out |= 1 << idx;
            }
        }
        mask &= ~bit;
        track++;
        bit <<= 1;
    } while (mask != 0);

    *mask_out &= mask_keep;
}

void Akao_SetVoicePitch(void) {
    unsigned secondary, primary, keep;
    AkaoSequencerBank *bank;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_request_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        secondary &= ~bank->pending_voice_mask;
        g_AkaoCurTrack--;
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_request_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        primary &= ~bank->pending_voice_mask;
        /* Preserve the retail reload before clearing the bank requests. */
        __asm__("" ::: "memory");
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
        g_AkaoCurTrack--;
    }
    if (primary) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
    }
    result |= g_SpuPendingKeyOffMask;
    g_SpuPendingKeyOffMask = 0;
    if (result) Spu_WriteKeyOff(result);
}

void Seq_MarkTrack34MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceVolume(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack34Mask;
    D_800C0DD4 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack38MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceAdsr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].volume_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->volume_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack38Mask;
    D_800C0DD0 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack3CMaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceStartAddr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].adsr_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->adsr_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack3CMask;
    D_800C0DD8 = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}
