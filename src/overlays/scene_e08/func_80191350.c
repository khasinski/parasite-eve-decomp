typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;

int func_80077DC4(int angle);

typedef struct {
    char pad_00[1];
    u8 status;
    s16 angle;
} Ovl178StepInput;

typedef struct {
    char pad_00[8];
    s16 field_08;
    s16 field_0A;
    u8 field_0C;
} Ovl178StepState;

void func_80191350(void *arg0, Ovl178StepInput *input, Ovl178StepState *state) {
    register Ovl178StepInput *input_reg __asm__("s2");
    register Ovl178StepState *state_reg __asm__("s0");
    register Ovl178StepState *check_state __asm__("s1");
    int delta;
    register int value __asm__("v1");
    register int timer __asm__("a0");
    register int phase __asm__("a1");

    input_reg = input;
    state_reg = state;

    delta = func_80077DC4(input_reg->angle << 5) >> 6;
    check_state = state_reg;
    phase = state_reg->field_0C;
    value = (u16)state_reg->field_08;
    timer = (u16)state_reg->field_0A;
    phase += 1;
    value += delta;
    timer -= 4;

    state_reg->field_0A = timer;
    timer <<= 16;
    state_reg->field_08 = value;
    state_reg->field_0C = phase;
    if (timer < 0) {
        state_reg->field_0A = 0;
    }

    if (check_state->field_0A == 0) {
        input_reg->status = 2;
    }
}
