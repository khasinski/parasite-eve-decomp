typedef struct Overlay002FadeState {
    char pad0[0x1C];
    int step_value;
    char pad20[0x10];
    int finished;
} Overlay002FadeState;

void func_8018FE1C(void);

void func_8012B754(Overlay002FadeState *state) {
    int step_value;

    step_value = state->step_value - 0x10;
    state->step_value = step_value;

    if (step_value == 0x80) {
        func_8018FE1C();
    }

    if (state->step_value == 0) {
        state->finished = 1;
    }
}
