#include "menu_memcard_fade.h"

s32 func_8012B53C(MenuMemcardFadeState *state) {
    s32 shifted;
    s32 result;

    state->displayTop = state->velocity + state->increment;
    shifted = state->increment << 4;
    if (shifted >= 0) {
        result = 0x100 - shifted;
    } else {
        {
            register s32 nextLevel asm("$2") = shifted + 0x100;
            asm volatile("" : : "r"(nextLevel));
            state->level = nextLevel;
        }
        state->increment += (state->increment != 0);
        if (state->onStep != 0 && state->increment == state->callbackAt) {
            state->onStep(state);
        }
        if (state->increment < 16) return 1;
        state->finished = 1;
        result = 1;
    }
    return result;
}
