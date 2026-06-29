/* MASPSX_FLAGS: --expand-div */

#include "pe1/akao_script.h"

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
