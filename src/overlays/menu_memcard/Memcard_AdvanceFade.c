#include "menu_memcard_fade.h"

void Memcard_AdvanceFade(MenuMemcardFadeState *state) {
    s32 level = state->level;

    if ((level <= 0 && state->velocity < 0) ||
        (level >= 0x100 && state->velocity > 0)) {
        state->velocity = 0;
    }
    state->level += state->velocity;
}
