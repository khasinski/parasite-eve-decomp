/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/akao/voice_state.h"
#include "pe1/akao/voice_masks.h"

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
    u32 flags;
    u16 value;

    if (g_AkaoCurTrack->active_voice_mask != 0) {
        Util_CopyWords((u32 *)g_AkaoCurTrack, (u32 *)&g_AkaoTrackStateBackup, sizeof(AkaoSequencerBank));
        Util_CopyWords((u32 *)g_AkaoVoiceStateTable, (u32 *)&g_AkaoVoiceStateBackup, sizeof(AkaoVoiceBank));

        flags = g_AkaoTrackStateBackup.status_flags;
        if ((flags & 0x100) != 0) {
            i = 0;
            track = g_AkaoVoiceStateBackup.tracks;
            do {
                value = track->note_pitch;
                if (value >= 0x50) {
                    track->note_pitch = value - 0x30;
                }
                i++;
                track++;
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

typedef unsigned short u16_1;


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
