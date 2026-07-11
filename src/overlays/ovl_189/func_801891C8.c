typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

typedef struct {
    char bytes[8];
} Overlay189Blob8;

typedef struct {
    s16 h0;
    u16 h2;
    char pad4[0x2];
    s16 h6;
    s16 h8;
    s16 hA;
} Overlay189Effect;

extern Overlay189Blob8 D_8018F1F4;
extern s32 D_800E27EC;
extern u16 D_800F336C;
extern u16 D_800E1204[];
extern s32 D_800F3428;

s32 func_80077DC4(s32 arg0);
s32 func_80077AA4(s32 arg0, s32 arg1);
s32 func_800CEE20(Overlay189Effect *arg0, void *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8);

s32 func_801891C8(s32 mode, Overlay189Effect *effect) {
    Overlay189Blob8 blob;
    s32 value;
    s32 scale;
    s32 tex;
    s32 mag;

    blob = D_8018F1F4;
    if (mode == 1) {
        goto mode1;
    }
    if (mode == 2) {
        goto mode2;
    }
    {
        register s32 ret asm("$2");

        ret = 0;
        asm volatile(
            ".set\tnoreorder\n\t"
            "j\tfunc_80198330\n\t"
            "nop\n\t"
            ".set\treorder"
            :
            : "r"(ret));
    }

mode1:
    if (effect->h8 != 0) {
        return 0;
    }
    effect->hA++;
    {
        register s32 step asm("$3");
        register s32 dec asm("$2");

        step = effect->h6;
        step = (step * 3) << 3;
        if (step < 0) {
            step += 0xFFF;
        }
        dec = step >> 12;
        effect->h2 -= dec;
    }
    if (effect->hA < 0x20) {
        return 0;
    }
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\tfunc_80198330\n\t"
        "li\t$2, 1\n\t"
        ".set\treorder");

mode2:
    if (effect->h8 != 0) {
        return 0;
    }
    *(s16 *)&blob.bytes[4] = D_800E27EC << 5;
    mag = func_80077DC4(effect->hA << 5);
    value = effect->h6 * mag;
    if (value < 0) {
        value += 0xFFF;
    }
    scale = value >> 12;
    tex = D_800E1204[D_800F336C];
    if (D_800F336C == 4 && D_800F3428 != 0) {
        tex += 4;
    }
    func_800CEE20(effect, &blob, scale, scale, 0x22, (u16) func_80077AA4(0x70, tex), 0xFF, 0x80, 0);
    return 0;
}
