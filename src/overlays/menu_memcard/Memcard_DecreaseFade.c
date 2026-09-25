#include "menu_memcard_fade.h"

void Memcard_DecreaseFade(MenuMemcardFadeState *state) {
    s32 level = state->level - 0x10;

    if (level < 0) {
        level = 0;
    }
    state->level = level;
    if (level == 0) {
        state->increment = 0;
        state->velocity = 0;
    }
}
