/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"
#include "pe1/akao_script.h"


void SeqOp_SetPanSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    delta = ((pc[1] << 8) - state->pan_current) / duration;
    state->pan_duration = duration;
    state->pan_delta = delta;
}


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


void SeqOp_SetExpressionSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    delta = ((pc[1] << 8) - state->expr_current) / duration;
    state->expr_duration = duration;
    state->expr_delta = delta;
}


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


void SeqOp_PitchSlide(AkaoScriptState *state) {
    u8 *pc = state->pc;
    int duration;
    int target_delta;

    state->pc = pc + 1;
    duration = pc[0];
    if (duration == 0) {
        duration = 0x100;
    }
    state->pc = pc + 2;
    target_delta = ((pc[1] << 7) - state->pitch_current) / duration;
    state->pitch_duration = duration;
    state->pitch_delta = target_delta;
}
