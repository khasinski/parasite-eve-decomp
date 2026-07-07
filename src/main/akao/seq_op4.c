typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#include "pe1/akao.h"

extern void *g_AkaoSoundEntryTable[];

void SeqOp_ResetTrack(AkaoTrack *track) {
    track->volume_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_VOLUME_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
}

void SeqOp_LoadSoundEntry(AkaoTrack *track) {
    u8 *pc;
    int duration;
    int selector;

    track->flags |= AKAO_TRACK_FLAG_AUX_LFO;

    pc = track->pc;
    track->pc = pc + 1;
    duration = pc[0];
    track->aux_lfo_duration = duration;
    if (duration == 0) {
        track->aux_lfo_duration = 0x100;
    }

    pc = track->pc;
    track->pc = pc + 1;
    selector = pc[0];
    track->aux_lfo_selector = selector;
    {
        void *entry = g_AkaoSoundEntryTable[selector];
        track->aux_lfo_phase = 1;
        track->aux_lfo_table = entry;
    }
}

void SeqOp_SetPitchBase(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->aux_lfo_target = *cursor << 7;
}
