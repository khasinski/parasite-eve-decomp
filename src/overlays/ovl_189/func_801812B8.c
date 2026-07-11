typedef unsigned char u8;
typedef int s32;

typedef struct {
    char pad0[0x8];
    u8 *data;
    void (*callback)(void *);
} Overlay189StepState;

s32 func_800DFB78(void);
void func_801909E0(Overlay189StepState *arg0);

s32 func_801812B8(Overlay189StepState *arg0) {
    s32 step;

    step = func_800DFB78();
    if (step == 1) {
        goto step_one;
    }
    if (step >= 2) {
        goto done;
    }
    if (step != 0) {
        goto done;
    }
    if (arg0->data[0xE] < 2) {
        goto done;
    }
    arg0->callback(arg0);
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_80190314\n\t"
        "move\t$2, $0\n\t"
        ".set\treorder");

step_one:
    func_801909E0(arg0);

done:
    return 0;
}
