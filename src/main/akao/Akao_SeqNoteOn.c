#include "common.h"
#include "pe1/akao.h"

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
#include "common.h"
#include "pe1/akao.h"

void SeqOp_SetTrack38Mask();

extern void *g_AkaoCurTrack[];
#define g_AkaoCurTrack (g_AkaoCurTrack[0])

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
