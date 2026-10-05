/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"
#include "pe1/akao_script.h"
typedef signed int s32;
extern char *g_AkaoCurTrack;
void SeqOp_SetTrack38Mask();


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


void sndTrackReadAdsrDecayRate(void *ptr);
void sndTrackReadAdsrSustainLevel(void *ptr, int arg);

void SeqOp_SetTrack34Mask(void *ptr);

void SeqOp_SetMask(void *ptr);

void SeqOp_ClearTrack34Mask(void *ptr);
void SeqOp_ClearTrack3CMask(void *ptr, int arg);
void SeqOp_ClearMask(void *ptr, int arg);

extern u32 D_800B89D0;
extern u32 D_800B89D4;
extern u32 D_800B89D8;
extern u32 D_800B89DC;
extern u32 D_800B89E0;

void Seq_StartNestedStreams(void *ptr, void *first, void *second);

void SeqOp_ReadAdsrDecayAndSustain(void *ptr, int arg) {
    sndTrackReadAdsrDecayRate(ptr);
    sndTrackReadAdsrSustainLevel(ptr, arg);
}

void SeqOp_SetDurationBAAndMask(AkaoTrack *track) {
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
    SeqOp_SetTrack34Mask(track);
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

void SeqOp_SetRepeatCount(AkaoTrack *track) {
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
    SeqOp_SetMask(track);
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

void SeqOp_StopAndClearTrack(AkaoTrack *track, int arg) {
    track->flags &= ~AKAO_TRACK_FLAG_STOP_CLEAR_MASK;
    SeqOp_ClearTrack34Mask(track);
    SeqOp_ClearTrack3CMask(track, arg);
    SeqOp_ClearMask(track, arg);
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

    D_800B89D4 = 0;
    D_800B89D8 = 0;
    D_800B89DC = *(u16 *)((char *)track + 0x76) >> 8;
    D_800B89E0 = *(int *)((char *)track + 0x44) >> 23;
    Seq_StartNestedStreams(&D_800B89D0, first, second);
    *(u8 **)track += 4;
}





void SeqOp_NoteOnWithVoiceAlloc(AkaoTrack *track, s32 arg1, s32 arg2, s16 arg3) {
    register s16 voice_index asm("$7");
    register s32 voice_mask asm("$6");
    s32 used_mask;
    register s32 valid_mask asm("$8");
    s32 flags;
    unsigned char *cursor;

    voice_mask = arg2;
    voice_index = arg3;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->flags;
    track->pitch_slide_current = *cursor << 8;
    track->pitch_slide_duration = 0;
    if (!(flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED)) {
        voice_index = 0;
        voice_mask = 1;
        valid_mask = 0xFFFFFF;
        used_mask = *(s32 *)(g_AkaoCurTrack + 4) | *(s32 *)(g_AkaoCurTrack + 0x30);
loop_2:
        if (used_mask & voice_mask) {
            voice_mask *= 2;
            voice_index += 1;
            if (!(voice_mask & valid_mask)) {

            } else {
                goto loop_2;
            }
        }
        if (voice_mask & 0xFFFFFF) {
            *(s32 *)(g_AkaoCurTrack + 0x30) |= voice_mask;
            track->voice_index = voice_index;
            track->flags |= AKAO_TRACK_FLAG_VOICE_ALLOCATED;
        }
    }
    SeqOp_SetTrack38Mask();
}


extern u32 *D_8009D2C8;



void SeqOp_NoteOnWithPitchSlide(void *track) {
    register AkaoTrack *base asm("$6");
    register u8 *seq_first asm("$2");
    u8 *seq;
    int value;
    int step_count;
    int pitch_base;
    int mask;
    register int voice_index asm("$7");
    int check;
    base = (AkaoTrack *)track;
    asm volatile("" : "=r"(base) : "0"(base));

    seq_first = base->pc;
    base->pc = seq_first + 1;
    step_count = seq_first[0];
    base->pitch_slide_duration = step_count;
    if (step_count == 0) {
        step_count = 0x100;
        base->pitch_slide_duration = step_count;
    }

    seq = base->pc;
    pitch_base = base->pitch_slide_current & 0xFF00;
    base->pc = seq + 1;
    value = ((int)(seq[0] << 24) >> 16) - pitch_base;
    base->pitch_slide_current = pitch_base;
    base->pitch_slide_delta = value / base->pitch_slide_duration;

    if ((base->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) == 0) {
        voice_index = 0;
        mask = 1;
        check = (int)D_8009D2C8;
        {
            register unsigned int limit asm("$8");
            unsigned int used = ((u32 *)check)[1] | ((u32 *)check)[12];
            limit = 0xFFFFFF;
scan_voice:
            if ((used & mask) != 0) {
                mask <<= 1;
                voice_index++;
                if ((mask & limit) != 0)
                    goto scan_voice;
            }
            check = mask & 0xFFFFFF;
        }

        if (check != 0) {
            D_8009D2C8[0xC] |= mask;
            base->voice_index = voice_index;
            base->flags |= AKAO_TRACK_FLAG_VOICE_ALLOCATED;
        }
    }

    SeqOp_SetTrack38Mask(base);
}


extern u32 g_AkaoVoiceUpdateFlags;
extern int g_AkaoSeqLoopCounter;

void SeqOp_DeactivateVoice(char *track);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void SeqOp_SetSustainFlag(AkaoTrack *track) {
    track->flags |= AKAO_TRACK_FLAG_SUSTAIN;
}

void SeqOp_StopVoice(AkaoTrack *track, u32 arg1) {
    char *state;
    u32 mask;

    if (track->parent_track_id == 0) {
        state = g_AkaoCurTrack;
        mask = arg1 ^ ~0xFF000000u;

        *(u32 *)(state + 4) &= mask;
        if (*(u32 *)(state + 4) == 0) {
            g_AkaoSeqLoopCounter = 0;
            *(u16 *)(state + 0x54) = 0;
        }

        state = g_AkaoCurTrack;
        {
            u32 temp0;
            u32 temp1;

            temp0 = *(u32 *)(state + 8);
            temp1 = *(u32 *)(state + 0x34);
            temp0 &= mask;
            *(u32 *)(state + 8) = temp0;
            temp0 = *(u32 *)(state + 0xC);
            temp1 &= mask;
            *(u32 *)(state + 0x34) = temp1;
            temp1 = *(u32 *)(state + 0x3C);
            temp0 &= mask;
            *(u32 *)(state + 0xC) = temp0;
            temp0 = *(u32 *)(state + 0x38);
            temp1 &= mask;
            *(u32 *)(state + 0x3C) = temp1;
            temp0 &= mask;
            *(u32 *)(state + 0x38) = temp0;
        }

        if ((track->flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) != 0) {
            *(u32 *)(state + 0x30) &= ~(1 << track->voice_index);
        }
    } else {
        SeqOp_DeactivateVoice((char *)track);
    }

    *(u32 *)((char *)track + 0x38) = 0;
    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}

void SeqOp_Noop(AkaoTrack *track, u32 mask) {
    SeqOp_StopVoice(track, mask);
}
