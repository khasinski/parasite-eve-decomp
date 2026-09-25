#include "menu_memcard_fade.h"

void func_8018FE1C(void);

void func_8012B754(MenuMemcardFadeState *state) {
    int step_value;

    step_value = state->level - 0x10;
    state->level = step_value;

    if (step_value == 0x80) {
        func_8018FE1C();
    }

    if (state->level == 0) {
        state->finished = 1;
    }
}
