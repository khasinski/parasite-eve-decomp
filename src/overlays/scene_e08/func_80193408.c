typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;

typedef struct {
    char pad_00[1];
    s8 status;
    s16 timer;
} Ovl178InputState;

typedef struct {
    s16 field_00;
    s16 field_02;
    char pad_04[6];
    s16 field_0A;
    char pad_0C[4];
    s16 field_10;
    char pad_12[2];
    s16 field_14;
    char pad_16[2];
    s16 field_18;
} Ovl178TransformState;

void func_80193408(void *arg0, Ovl178InputState *input, Ovl178TransformState *state) {
    register int value __asm__("v0");
    register int other __asm__("v1");

    value = (u16)state->field_0A + 0xC8;
    state->field_0A = value;

    if (input->timer < 0x10) {
        value = (u16)state->field_18 + 8;
        other = (u16)state->field_02 + 0x100;
        state->field_18 = value;
        state->field_02 = other;
    } else {
        value = (u16)state->field_18 - 4;
        other = (u16)state->field_14 + 0x50;
        state->field_18 = value;
        value = (u16)state->field_10;
        state->field_14 = other;
    }

    value += 0x50;
    state->field_10 = value;
    value = state->field_18;
    if (value <= 0) {
        input->status = 2;
    }
}
