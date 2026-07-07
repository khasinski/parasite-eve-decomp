#include "pe1/akao.h"
#include "pe1/akao_script.h"

extern AkaoTrack *g_AkaoCurTrack;

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
    AkaoTrack *track = g_AkaoCurTrack;

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
    AkaoTrack *track = g_AkaoCurTrack;

    *stream = cursor + 1;
    track->repeat_counters[1] = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    track->repeat_counters[1] |= cursor[0] << 8;
}
