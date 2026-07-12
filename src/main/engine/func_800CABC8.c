typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    short vx;
    short vy;
    short vz;
    short pad;
} SVECTOR;

void ApplyMatrixSV(void *matrix, void *in, SVECTOR *out);

extern char *D_8009D254;
extern char D_800E0C48[];
extern char *D_800E27A8;
extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;

int func_800CABC8(void *arg0, void *arg1, u8 *anim) {
    register u8 *anim_s0 asm("$16") = anim;
    SVECTOR out;
    char *data;
    char *entry;
    char *model;
    int m0;
    int m1;
    int m2;
    int m3;

    entry = D_800E0C48 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, entry, &out);

    *(u16 *)(anim_s0 + 0x8) = D_800E2360 + out.vx;
    *(u16 *)(anim_s0 + 0xA) = D_800E2362 + out.vy;
    data = D_800E27A8;
    *(u16 *)(anim_s0 + 0xC) = D_800E2364 + out.vz;

    model = *(char **)(data + 0x238);
    m0 = *(int *)(model + 0x260);
    m1 = *(int *)(model + 0x264);
    m2 = *(int *)(model + 0x268);
    m3 = *(int *)(model + 0x26C);
    *(int *)(anim_s0 + 0x10) = m0;
    *(int *)(anim_s0 + 0x14) = m1;
    *(int *)(anim_s0 + 0x18) = m2;
    *(int *)(anim_s0 + 0x1C) = m3;
    m0 = *(int *)(model + 0x270);
    m1 = *(int *)(model + 0x274);
    m2 = *(int *)(model + 0x278);
    m3 = *(int *)(model + 0x27C);
    *(int *)(anim_s0 + 0x20) = m0;
    *(int *)(anim_s0 + 0x24) = m1;
    *(int *)(anim_s0 + 0x28) = m2;
    *(int *)(anim_s0 + 0x2C) = m3;
    asm volatile("" ::: "memory");
    *(u16 *)(anim_s0 + 0x4) = 0x7F;
    *(u16 *)(anim_s0 + 0x6) = 0x224;
}
