typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    char pad_00[1];
    u8 status;
    s16 target;
} Ovl178FadeInput;

typedef struct {
    char pad_000[0x230];
    s16 field_230[2];
    char pad_234[0x20];
    s16 field_254[4];
    s16 field_25C[4];
    s16 field_264;
} Ovl178FadeState;

void func_8018F8B8(void *arg0, Ovl178FadeInput *input, Ovl178FadeState *state) {
    unsigned int i;
    s16 value;

    for (i = 0; i < 4; i++) {
        state->field_254[i] = (u16)state->field_254[i] + 0x3C;
        state->field_25C[i] = (u16)state->field_25C[i] + 0x80;
    }

    for (i = 0; i < 2; i++) {
        if (state->field_230[i] < 0) {
            state->field_230[i] = 0;
        }
        state->field_230[i] = (u16)state->field_230[i] - 0x1E;
    }

    value = state->field_264;
    if (value >= 3) {
        state->field_264 = value - 2;
    }

    if (input->target == 0x28) {
        input->status = 2;
    }
}
