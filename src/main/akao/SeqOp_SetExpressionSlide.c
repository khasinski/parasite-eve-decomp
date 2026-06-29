/* MASPSX_FLAGS: --expand-div */

#include "pe1/akao_script.h"

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
