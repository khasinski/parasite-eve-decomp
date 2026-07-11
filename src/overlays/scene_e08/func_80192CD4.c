typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;

typedef struct {
    char pad_00[1];
    s8 status;
    s16 value;
} Ovl178InputState;

typedef struct {
    char pad_00[2];
    s16 value;
    s16 mode;
} Ovl178RampState;

void func_80192CD4(void *arg0, Ovl178InputState *input, Ovl178RampState *ramp) {
    register int mode __asm__("v1");
    int value;

    /* Preserve the original branch delay slot and v0 store sequence. */
    __asm__ volatile(
        "lh    $v0, 0x2(%0)\n"
        "nop\n"
        "slti  $v0, $v0, 0x3D\n"
        "bnez  $v0, 1f\n"
        " addiu $v0, $zero, 0x1\n"
        "addiu $v0, $zero, 0x2\n"
        "1:\n"
        "sh    $v0, 0x4(%1)"
        :
        : "r"(input), "r"(ramp)
        : "v0", "memory");

    mode = ramp->mode;
    if (mode == 1) {
        value = (u16)ramp->value + 8;
        ramp->value = value;
        if ((s16)value >= 0x80) {
            ramp->value = 0x7F;
        }
        mode = ramp->mode;
    }

    if (mode == 2) {
        value = (u16)ramp->value - 8;
        ramp->value = value;
        if ((value << 0x10) < 0) {
            ramp->value = 0;
        }
    }

    if (input->value >= 0x79) {
        input->status = 2;
    }
}
