typedef unsigned int u32;

#include "pe1/akao.h"

extern char *g_AkaoCurTrack;
extern u32 g_AkaoVoiceUpdateFlags;
extern u32 g_AkaoTrack34Mask;

void Seq_MarkTrack34MaskDirty(void);

void SeqOp_ResetFlag4State(AkaoTrack *track) {
    track->aux_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_AUX_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
}

void SeqOp_SetTrack34Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_a |= mask;
    } else {
        g_AkaoTrack34Mask |= mask;
    }

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
}

void SeqOp_ClearTrack34Mask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_a &= ~mask;
    } else {
        g_AkaoTrack34Mask &= ~mask;
    }

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    track->key_on_delay = 0;
}
