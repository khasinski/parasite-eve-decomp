typedef signed short s16;

typedef struct {
    s16 field_00;
    s16 field_02;
    s16 field_04;
    s16 pad_06;
    s16 field_08;
    s16 field_0A;
    s16 field_0C;
    s16 pad_0E;
    s16 field_10;
    s16 field_12;
    s16 field_14;
    s16 pad_16;
    s16 field_18;
} Ovl178TransformInit;

void func_8019311C(void *arg0, void *arg1, Ovl178TransformInit *state) {
    register int scale __asm__("v1");
    register int value __asm__("v0");

    scale = 0x270;
    value = -0x1000;
    __asm__ volatile("" : : "r"(scale));
    state->field_02 = value;
    value = 0x1000;
    state->field_12 = value;
    value = 1;
    state->field_08 = 0;
    state->field_0A = 0;
    state->field_0C = 0;
    state->field_00 = 0;
    state->field_04 = 0;
    state->field_10 = scale;
    state->field_14 = scale;
    state->field_18 = value;
}
