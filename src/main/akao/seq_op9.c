#include "pe1/akao.h"

typedef unsigned int u32;
typedef unsigned short u16;

extern char *g_AkaoCurTrack;
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
