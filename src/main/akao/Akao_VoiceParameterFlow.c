/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/akao/voice_state.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/akao.h"
typedef unsigned short u16_1;

void Akao_UpdateVoiceMask(int new_base) {
    AkaoTrack *track;
    AkaoTrack *active;
    AkaoSequencerBank *bank;
    register unsigned int bit asm("$7");
    register unsigned int remaining asm("$8");
    unsigned int mask;
    unsigned int voice_mask;
    register unsigned int updated asm("$2");
    int delta;
    int backup_base;
    unsigned int old_status, allocation;
    AkaoSequencerBank *loaded_bank;
    unsigned int key_on_state;
    unsigned int valid_bits;
    register unsigned int pending asm("$3");
    unsigned int restore_mask;
    AkaoSequencerBank *read_bank;
    AkaoSequencerBank *clear_bank;
    AkaoSequencerBank *final_bank;
    register AkaoU16 *duration asm("$5");
    unsigned int magic_flags;
    register unsigned int reset_duration asm("$10");

    Util_CopyWords((unsigned int *)&g_AkaoTrackStateBackup,
                   (unsigned int *)g_AkaoCurTrack, 0x68);
    Util_CopyWords((unsigned int *)&g_AkaoVoiceStateBackup,
                   (unsigned int *)&g_AkaoVoiceStateTable, sizeof(AkaoVoiceBank));

    track = g_AkaoVoiceStateTable;
    remaining = AKAO_VOICE_COUNT;
    bit = 1;
    magic_flags = 0x1ff93;
    reset_duration = 4;
    duration = &track->pan_duration;
    /* Keep the loop constants live before loading the sequencer bank. */
    asm volatile("" : "=r"(remaining), "=r"(bit), "=r"(magic_flags),
                      "=r"(reset_duration), "=r"(duration)
                    : "0"(remaining), "1"(bit), "2"(magic_flags),
                      "3"(reset_duration), "4"(duration));
    loaded_bank = g_AkaoCurTrack;
    key_on_state = g_AkaoVoiceKeyOnState;
    asm("" : "=r"(bank) : "0"(loaded_bank), "r"(key_on_state));
    old_status = bank->status_flags;
    allocation = bank->allocated_voice_mask;
    bank->field_20[3] = new_base;
    key_on_state &= 0x100;
    old_status |= key_on_state;
    bank->status_flags = old_status;
    bank->key_on_request_mask = allocation;
    backup_base = g_AkaoTrackStateBackup.field_20[3];
    asm volatile("" : "=r"(backup_base) : "0"(backup_base));
    delta = new_base - backup_base;
    g_AkaoVoiceUpdateFlags |= 0x90;
    asm volatile("" ::: "memory");
    voice_mask = bank->active_voice_mask;
    do {
        /* The original loop addresses these fields from pan_duration. */
        active = (AkaoTrack *)((char *)duration - 0x58);
        if (voice_mask & bit) {
            track->pc += delta;
            active->branch_target += delta;
            active->call_stack[0] += delta;
            active->call_stack[1] += delta;
            active->call_stack[2] += delta;
            active->call_stack[3] += delta;
            duration[-1] += 2;
            duration[0] += 2;
            active->update_flags |= magic_flags;
            if ((bank->status_flags & 0x100) && duration[1] >= 0x20) {
                duration[1] += 0x30;
            }
        } else {
            register AkaoU8 *default_pc asm("$2");
            register AkaoU16 short_duration asm("$2");
            /* These pins keep the constants inside this branch. */
            duration[-1] = reset_duration;
            short_duration = 2;
            duration[0] = short_duration;
            default_pc = (AkaoU8 *)g_AkaoDefaultVoiceProgram;
            track->pc = default_pc;
        }
        --remaining;
        duration += sizeof(AkaoTrack) / sizeof(AkaoU16);
        track++;
        bit <<= 1;
    } while (remaining != 0);

    read_bank = g_AkaoCurTrack;
    updated = Akao_ForEachVoiceMasked(g_AkaoVoiceStateTable2,
                                      ((AkaoSequencerState *)read_bank)->secondary.active_voice_mask &
                                      ((AkaoSequencerState *)read_bank)->secondary.pending_voice_mask);
    valid_bits = 0xffffff;
    clear_bank = g_AkaoCurTrack;
    updated = ~updated;
    clear_bank->key_off_request_mask = 0;
    asm volatile("" ::: "memory");
    mask = ~g_SpuActiveVoiceMask;
    mask &= valid_bits;
    updated &= mask;
    pending = g_SpuPendingKeyOffMask;
    pending |= updated;
    g_SpuPendingKeyOffMask = pending;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
    g_AkaoSelectedBankId = 0;
    if (g_AkaoSeqPendingFlags & 1) {
        final_bank = g_AkaoCurTrack;
        restore_mask = final_bank->active_voice_mask;
        final_bank->active_voice_mask = 0;
        final_bank->pending_restore_mask = restore_mask;
    }
}


void Akao_UpdateVoiceMask(int arg0);
void Akao_StepSequencerVoice(void *arg0);

void Seq_SelectPlaybackBank(int *arg0) {
    u16 value;

    value = g_AkaoSelectedBankId;
    if ((value != 0) && (value == arg0[3])) {
        Akao_UpdateVoiceMask(arg0[1]);
    } else {
        Akao_StepSequencerVoice((void *)arg0[1]);
        ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = arg0[3];
    }
}

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);

void Seq_RestorePrimaryState(void) {
    u32 i;
    AkaoTrack *track;
    u16 *pitch;
    u32 flags;
    u16 value;

    if (g_AkaoCurTrack->active_voice_mask != 0) {
        Util_CopyWords((u32 *)g_AkaoCurTrack, (u32 *)&g_AkaoTrackStateBackup, sizeof(AkaoSequencerBank));
        Util_CopyWords((u32 *)g_AkaoVoiceStateTable, (u32 *)&g_AkaoVoiceStateBackup, sizeof(AkaoVoiceBank));

        flags = g_AkaoTrackStateBackup.status_flags;
        if ((flags & 0x100) != 0) {
            i = 0;
            track = g_AkaoVoiceStateBackup.tracks;
            pitch = &track->note_pitch;
            do {
                value = *pitch;
                if (value >= 0x50) {
                    *pitch = value - 0x30;
                }
                i++;
                pitch += sizeof(AkaoTrack) / sizeof(u16);
            } while (i < 24);
        }
    }
}


void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);
void Akao_StepSequencerVoice(void *arg0);

extern int g_AkaoSeqLoopCounter;
void Seq_SelectPlaybackBank(int *arg0);


void Seq_RestoreSecondaryStateAndSelect(int *arg0) {
    AkaoSequencerBank *state;

    state = (AkaoSequencerBank *)g_AkaoCurTrack;
    if ((state->active_voice_mask != 0) && ((state + 1)->active_voice_mask == 0)) {
        Util_CopyWords((u32 *)state, (u32 *)(state + 1), sizeof(*state));
        Util_CopyWords((u32 *)g_AkaoVoiceStateTable,
                       (u32 *)&g_AkaoVoiceStateTable[AKAO_VOICE_COUNT], sizeof(AkaoVoiceBank));
    }

    Akao_StepSequencerVoice((void *)arg0[1]);
    ((AkaoTrack *)g_AkaoCurTrack)->parent_track_id = arg0[3];
}

void Seq_SelectPlaybackBankWithCountdown(int *arg0)
{
  int raw;
  register int value;
  Seq_SelectPlaybackBank(arg0);
  raw = arg0[4] & 0xFFFFFFFFFFFFFFFFu;
  value = 0;
  if (raw != 0)
  {
    value = raw - 1;
  }
  g_AkaoSeqLoopCounter = value;
}

void Seq_StartDefaultNestedStream(int *arg0) {
    int old1 = arg0[1];
    int old2 = arg0[2];

    arg0[1] = 0x400;
    arg0[2] = 0x1000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams((AkaoNestedSource *)arg0, (void *)old1, (void *)old2);
}
void Akao_LoadSamplePairFromIndex(int *arg0, int *arg1, int arg2);

void Seq_StartIndexedNestedStreamWithDefaults(int *arg0) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    arg0[2] = 0x2000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams((AkaoNestedSource *)arg0, (void *)old1, (void *)old2);
}

void Seq_StartIndexedNestedStream(int *arg0) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    Seq_StartNestedStreams((AkaoNestedSource *)arg0, (void *)old1, (void *)old2);
}


#define g_AkaoCurTrackBytes ((char *)g_AkaoCurTrack)
#define g_AkaoVoiceStateTableBytes ((char *)g_AkaoVoiceStateTable)
#define g_AkaoVoiceStateTable2Bytes ((char *)g_AkaoVoiceStateTable2)



void Seq_StartNestedStreams(AkaoNestedSource *arg0, void *arg1, void *arg2);

void Spu_ManageVoices(int arg0, int arg1);




void Seq_MarkDirtyTracks(char *arg0);

extern short g_AkaoGlobalPitchSlideCounter;
extern int D_8009D2B4;

void Seq_ApplyGlobalPitch(void);

void Seq_StartRelativeNestedStream(void *arg0) {
    char *base;
    u16 offset;
    void *arg1;
    void *arg2;

    base = *(char **)((char *)arg0 + 4);
    offset = *(u16 *)base;
    if (offset != 0xFFFF) {
        arg1 = (char *)(offset + (int)base) + 4;
    } else {
        arg1 = 0;
    }

    offset = *(u16 *)(base + 2);
    arg2 = 0;
    if (offset != 0xFFFF) {
        arg2 = (char *)(offset + (int)*(char **)((char *)arg0 + 4)) + 4;
    }

    *(void **)((char *)arg0 + 4) = *(void **)((char *)arg0 + 0x14);
    Seq_StartNestedStreams(arg0, arg1, arg2);
}

void Spu_ManageVoicesCmd(int *arg0) {
    Spu_ManageVoices(arg0[1], arg0[2]);
}

void Seq_SetTrackPitchImmediate(int *arg0) {
    int *msg;
    int selector;
    char *track;
    char *primary_track;
    char *next_track;
    char *base;

    msg = arg0;
    selector = msg[4];
    if (selector == 0 || selector == *(u16_1 *)(g_AkaoCurTrackBytes + 0x54)) {
        int value;
        base = g_AkaoVoiceStateTableBytes;
        value = msg[1];
        primary_track = g_AkaoCurTrackBytes;
        value &= 0x7F;
        value <<= 16;
        *(int *)(primary_track + 0x48) = value;
        *(u16_1 *)(primary_track + 0x50) = 0;
        Seq_MarkDirtyTracks(base);
    } else if (selector != 0) {
        track = g_AkaoCurTrackBytes;
        if (selector == *(u16_1 *)(track + 0xBC)) {
            int value;
            next_track = track + 0x68;
            base = g_AkaoVoiceStateTable2Bytes;
            value = msg[1];
            g_AkaoCurTrack = (AkaoSequencerBank *)next_track;
            *(u16_1 *)(track + 0xB8) = 0;
            value &= 0x7F;
            value <<= 16;
            *(int *)(track + 0xB0) = value;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack--;
        }
    }
}

void Seq_SlideTrackPitch(int *arg0) {
    register int raw asm("$2");
    int duration;
    register int target asm("$5");
    int selector;
    char *track;
    char *base;
    char *next_track;

    raw = arg0[1];
    duration = 1;
    if (raw != 0) {
        duration = raw;
    }

    raw = arg0[2];
    selector = arg0[4];
    raw &= 0x7F;
    target = raw << 16;

    if (selector != 0) {
        track = g_AkaoCurTrackBytes;
        if (selector != *(u16 *)(track + 0x54)) {
            goto secondary_track;
        }
    }

    {
        track = g_AkaoCurTrackBytes;
        target = (target - *(int *)(track + 0x48)) / duration;
        base = g_AkaoVoiceStateTableBytes;
        ((AkaoTrack *)track)->field_50_duration = duration;
        ((AkaoTrack *)track)->pitch_slide_step = target;
        Seq_MarkDirtyTracks(base);
    }
    goto done;

secondary_track:
    if (arg0 != 0) {
        if (selector == *(u16 *)(track + 0xBC)) {
            target = (target - *(int *)(track + 0xB0)) / duration;
            base = g_AkaoVoiceStateTable2Bytes;
            *(u16 *)(track + 0xB8) = duration;
            next_track = track + 0x68;
            g_AkaoCurTrack = (AkaoSequencerBank *)next_track;
            *(int *)(track + 0xB4) = target;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack--;
        }
    }

done:
    return;
}

void Seq_TrackPitchSetup(int *arg0) {
    register int raw asm("$2");
    int duration;
    int selector;
    char *track;
    int start;
    register int delta asm("$6");
    char *base;
    char *next_track;

    raw = arg0[1];
    duration = 1;
    if (raw != 0) {
        duration = raw;
    }

    selector = arg0[4];
    if (selector != 0) {
        track = g_AkaoCurTrackBytes;
        if (selector != *(u16 *)(track + 0x54)) {
            goto secondary_track;
        }
    }

    {
        start = arg0[2];
        track = g_AkaoCurTrackBytes;
        start &= 0x7F;
        start <<= 16;
        *(int *)(track + 0x48) = start;
        raw = arg0[3];
        raw &= 0x7F;
        delta = raw << 16;
        delta -= start;
        delta = delta / duration;
        base = g_AkaoVoiceStateTableBytes;
        ((AkaoTrack *)track)->field_50_duration = duration;
        ((AkaoTrack *)track)->pitch_slide_step = delta;
        Seq_MarkDirtyTracks(base);
    }
    goto done;

secondary_track:
    if (selector != 0) {
        if (selector == *(u16 *)(track + 0xBC)) {
            start = arg0[2];
            start &= 0x7F;
            start <<= 16;
            *(int *)(track + 0xB0) = start;
            raw = arg0[3];
            raw &= 0x7F;
            delta = raw << 16;
            delta -= start;
            delta = delta / duration;
            base = g_AkaoVoiceStateTable2Bytes;
            *(u16 *)(track + 0xB8) = duration;
            next_track = track + 0x68;
            g_AkaoCurTrack = (AkaoSequencerBank *)next_track;
            *(int *)(track + 0xB4) = delta;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack--;
        }
    }

done:
    return;
}

void Seq_SetGlobalPitchImmediate(void *arg0) {
    int value;

    value = *(u16 *)((char *)arg0 + 4);
    g_AkaoGlobalPitchSlideCounter = 0;
    D_8009D2B4 = value << 16;
    Seq_ApplyGlobalPitch();
}

extern short g_AkaoGlobalPitchSlideCounter;
extern int g_AkaoGlobalPitchSlideStep;
extern int D_8009D2B4;


extern u32 g_SpuActiveVoiceMask;

void Seq_SlideGlobalPitchToTarget(void *arg0) {
    int duration;
    int raw_duration;
    int value;

    raw_duration = *(int *)((char *)arg0 + 4);
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    value = *(u16 *)((char *)arg0 + 8) << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    g_AkaoGlobalPitchSlideStep = (value - D_8009D2B4) / duration;
}

void Seq_SlideGlobalPitchFromStartToTarget(void *arg0) {
    int duration;
    int raw_duration;
    int start;
    int target;

    raw_duration = *(int *)((char *)arg0 + 4);
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    target = *(u16 *)((char *)arg0 + 0xC) << 16;
    start = *(u16 *)((char *)arg0 + 8) << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    D_8009D2B4 = start;
    g_AkaoGlobalPitchSlideStep = (target - start) / duration;
}

void Spu_SetVoiceVolumeImmediateMasked(int *arg0) {
    u32 i;
    u32 mask;
    u32 active;
    register char *voice;
    register char *base;
    int value;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = (arg0[3] & 0x7F) << 8;
                    *(u16_1 *)(voice - 0x80) = 0;
                    *(u16_1 *)(voice - 0x1C) = value;
                    *(u32 *)voice |= AKAO_VOICE_PARAM_VOLUME;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = (arg0[3] & 0x7F) << 8;
                    *(u16_1 *)(voice - 0x80) = 0;
                    *(u16_1 *)(voice - 0x1C) = value;
                    *(u32 *)voice |= AKAO_VOICE_PARAM_VOLUME;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern u32 g_SpuActiveVoiceMask;

static inline int ParameterDelta(int target, int current, int step) {
    return (short)(((target & 0x7F) << 8) - current) / (short)step;
}

void Spu_ParameterSlide(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int step;
    int delta;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x74;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x48) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ParameterDelta(arg0[4], *(u16 *)(voice + 0x64), step);
                    *(u16 *)(voice + 0x66) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x74;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x4C) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ParameterDelta(arg0[4], *(u16 *)(voice + 0x64), step);
                    *(u16 *)(voice + 0x66) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoiceVolumeImmediate(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int value;
    int dirty;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = g_SpuVoiceControlTable;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0xC8) & block_flag) == 0) {
                value = arg0[1];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x80) = 0;
                value &= 0x7F;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_VOLUME;
                *(u16 *)(voice - 0x1C) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern char D_800BC074[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SlideAllVoiceVolume(int *arg0) {
    u32 mask;
    u32 active;
    u32 block_flag;
    u32 i;
    register char *voice;
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC074;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x48) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)(((arg0[2] & 0x7F) << 8) - *(u16 *)(voice + 0x64));
                *(u16 *)voice = step;
                *(u16 *)(voice + 0x66) = delta / (short)step;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

void Spu_SetVoicePanImmediateMasked(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int value;
    int dirty;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(u32 *)voice;
                    *(u16 *)(voice - 0x7C) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    *(u16 *)(voice - 0x7E) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(u32 *)voice;
                    *(u16 *)(voice - 0x7C) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    *(u16 *)(voice - 0x7E) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

static inline int PanDelta(int target, int current, int step) {
    return (short)((target << 8) - current) / (short)step;
}

void Spu_SlideVoicePanMasked(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int step;
    int delta;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x78;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x4C) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], *(u16 *)(voice - 2), step);
                    *(u16 *)(voice + 0x64) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x78;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x50) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PanDelta(((u8 *)arg0)[0x10], *(u16 *)(voice - 2), step);
                    *(u16 *)(voice + 0x64) = delta;
                    *(u16 *)voice = step;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoicePanImmediate(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int value;
    int dirty;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = g_SpuVoiceControlTable;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0xC8) & block_flag) == 0) {
                value = ((u8 *)arg0)[4];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x7C) = 0;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_VOLUME;
                *(u16 *)(voice - 0x7E) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern char D_800BC078[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SlideAllVoicePan(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    char *voice;
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC078;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x4C) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = PanDelta(((u8 *)arg0)[8], *(u16 *)(voice - 2), step);
                *(u16 *)(voice + 0x64) = delta;
                *(u16 *)voice = step;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern u32 g_SpuActiveVoiceMask;

void Spu_SetVoicePitchImmediateMasked(int *arg0) {
    register char *base;
    u32 active;
    u32 mask;
    u32 i;
    register char *voice;
    int value;
    int dirty;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(volatile u32 *)voice;
                    *(u16 *)(voice - 0x84) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_PITCH;
                    *(u32 *)(voice - 0xB8) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(volatile u32 *)voice;
                    *(u16 *)(voice - 0x84) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_PITCH;
                    *(u32 *)(voice - 0xB8) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern u32 g_SpuActiveVoiceMask;

static inline short PitchDelta(int target, int current, int step) {
    return (short)((target << 8) - current) / (short)step;
}

void Spu_SlideVoicePitchMasked(int *arg0) {
    char *base;
    u32 active;
    u32 mask;
    u32 i;
    char *voice;
    int step;
    int delta;

    base = (char *)g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x70;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x44) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], *(int *)(voice - 0x34), step);
                    *(short *)voice = step;
                    *(int *)(voice - 0x30) = (short)delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x70;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x48) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = PitchDelta(((u8 *)arg0)[0x10], *(int *)(voice - 0x34), step);
                    *(short *)voice = step;
                    *(int *)(voice - 0x30) = (short)delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoicePitchImmediate(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int value;
    int dirty;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = g_SpuVoiceControlTable;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0xC8) & block_flag) == 0) {
                value = ((u8 *)arg0)[4];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x84) = 0;
                value <<= 8;
                dirty |= AKAO_VOICE_PARAM_PITCH;
                *(u32 *)(voice - 0xB8) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

extern char D_800BC070[];
extern u32 g_SpuActiveVoiceMask;

extern short g_AkaoGlobalD2D0SlideCounter;
extern int D_8009D2D0;

void Spu_SlideAllVoicePitch(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice;
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC070;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x44) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)((((u8 *)arg0)[8] << 8) - *(int *)(voice - 0x34));
                *(short *)voice = step;
                *(int *)(voice - 0x30) = (short)(delta / (short)step);
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}

void Akao_SetGlobalD2D0Immediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2D0SlideCounter = 0;
    D_8009D2D0 = value << 16;
}
