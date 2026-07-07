typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#include "pe1/akao.h"

extern void *g_AkaoSoundEntryTable[];

void SeqOp_StopPitchLFO(AkaoTrack *track) {
    track->pitch_lfo_value = 0;
    track->flags &= ~AKAO_TRACK_FLAG_PITCH_LFO;
    track->update_flags |= AKAO_VOICE_PARAM_PITCH;
}

void SeqOp_SetVolumeLFO(AkaoTrack *track) {
    u8 *pc;
    int value;
    int selector;
    int tmp;
    void *entry;

    track->flags |= AKAO_TRACK_FLAG_VOLUME_LFO;

    if (track->parent_track_id != 0) {
        track->volume_lfo_delay = 0;
        pc = track->pc;
        track->pc = pc + 1;
        value = pc[0];
        if (value != 0) {
            track->volume_lfo_target = value << 8;
        }
    } else {
        track->volume_lfo_delay = *track->pc++;
    }

    /* The chained assignment and the *(*pp)++ consume idiom are load-bearing:
     * they keep the stream pointer temp in $v0 as retail allocates it. */
    value = (track->volume_lfo_duration = *track->pc++);
    if (value == 0) {
        track->volume_lfo_duration = 0x100;
    }

    pc = track->pc;
    track->pc = pc + 1;
    selector = pc[0];
    tmp = track->volume_lfo_delay;
    track->volume_lfo_selector = selector;
    entry = g_AkaoSoundEntryTable[selector];
    track->volume_lfo_counter = tmp;
    track->volume_lfo_phase = 1;
    track->volume_lfo_table = entry;
}

void SeqOp_UpdateVolumeLFOTarget(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->volume_lfo_target = *cursor << 8;
}
