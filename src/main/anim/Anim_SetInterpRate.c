/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;

typedef struct {
    char pad[0x8D];
    u8 field8D;
    u8 field8E;
    u8 field8F;
    char pad90[3];
    u8 field93;
} Unk8003C5D8;

void Anim_SetInterpRate(Unk8003C5D8 *obj, int arg1) {
    register int original asm("$5") = arg1;
    int value = original;
    int divisor;

    original <<= 16;
    if (original == 0) {
        value = 1;
    }
    divisor = 0x80 / (short)value;

    obj->field8D = value;
    obj->field8E = divisor;
    obj->field8F = divisor;
    obj->field93 = divisor;
}
