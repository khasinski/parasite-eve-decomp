/* CC1_FLAGS: -G4 */

#include "pe1/akao_script.h"

extern void *g_AkaoCurTrack;

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

void SeqOp_ClearFlag3(void *ptr) {
    *(int *)((char *)ptr + 0x38) &= ~8;
}

void SeqOp_StreamPair(unsigned char **stream) {
    unsigned char *cursor = *stream;
    unsigned char value;
    char *track = g_AkaoCurTrack;

    *stream = cursor + 1;
    *(short *)(track + 0x60) = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    value = cursor[0];
    *(short *)(track + 0x62) = 0;
    *(short *)(track + 0x5E) = 0;
    *(short *)(track + 0x5C) = value;
}

void SeqOp_ReadTrack64U16(unsigned char **stream) {
    unsigned char *cursor = *stream;
    char *track = g_AkaoCurTrack;

    *stream = cursor + 1;
    *(unsigned short *)(track + 0x64) = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    *(unsigned short *)(track + 0x64) |= cursor[0] << 8;
}
