#include "pe1/scene_fx.h"

int func_800C6CE0(void);

int func_8018F190(SceneFxState *state) {
    state->mode = 4;
    if (func_800C6CE0() >= 2U) {
        state->holder->target->flags &= 0xC0FFFFFF;
    }
    return 0;
}
