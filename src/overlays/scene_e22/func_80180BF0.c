typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct {
    char pad0[0x28];
    char pos_28[0x10];
    char pos_38[0x2];
    s16 angle;
    char pos_3C[0x4];
    char pos_40[0x48];
} Overlay189Actor;

typedef struct {
    char pad0[0x28];
    s32 field_28;
    char pad2C[0x4];
    s32 field_30;
} Overlay189Target;

typedef struct {
    char pad0[0x60];
    s32 field_60;
    char pad64[0x4];
    s32 field_68;
    char pad6C[0x18];
    Overlay189Target *target;
    char pad88[0x10];
    s32 field_98;
    char pad9C[0x6];
    s16 field_A2;
    s16 field_A4;
    char padA6[0x3];
    u8 field_A9;
} Overlay189Mover;

s32 func_800DFF80(void *arg0, void *arg1);
s32 func_800DFFB8(s32 angle, s32 delta, s32 speed);
void func_800DFE94(void *arg0, void *arg1, void *arg2);

void func_80180BF0(Overlay189Actor *actor, Overlay189Mover *mover) {
    Overlay189Target *target;
    s32 delta;

    if (mover->field_A2 <= 0) {
        goto fallback;
    }

    target = mover->target;
    if (target != 0) {
        mover->field_60 = target->field_28;
        mover->field_68 = target->field_30;
    }
    delta = (s16) func_800DFF80(&mover->field_60, &actor->pos_28);
    actor->angle = func_800DFFB8(actor->angle, delta, mover->field_A2) & 0xFFF;
    if (mover->field_A9 == 0) {
        goto done;
    }
    if (mover->field_98 <= 0x7FFFFFF) {
        goto done;
    }
    mover->field_A2 = 0;
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_8018FCA0\n\t"
        "sh\t$0, 0xA4(%0)\n\t"
        ".set\treorder"
        :
        : "r"(mover));

fallback:
    func_800DFE94(&actor->pos_28, &actor->pos_40, &actor->pos_38);
    actor->angle = func_800DFFB8(actor->angle, actor->angle, mover->field_A4) & 0xFFF;

done:
    return;
}
