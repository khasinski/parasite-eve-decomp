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
