#include "pe1/akao_script.h"

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
