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
extern char D_800E08E8[];
extern char *D_800E279C;
extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;

int func_800C815C(void *arg0, void *arg1, u8 *anim) {
    register u8 *anim_s0 asm("$16") = anim;
    SVECTOR out;
    char *data;
    char *model;
    char *entry;
    int index;
    int m0;
    int m1;
    int m2;
    int m3;

    index = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1;
    index <<= 16;
    index >>= 13;
    entry = D_800E08E8 + index;
    ApplyMatrixSV(*(char **)(D_800E279C + 0x238) + 0x260, entry, &out);

    *(u16 *)(anim_s0 + 0x8) = D_800E2348 + out.vx;
    *(u16 *)(anim_s0 + 0xA) = D_800E234A + out.vy;
    data = D_800E279C;
    *(u16 *)(anim_s0 + 0xC) = D_800E234C + out.vz;

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
    *(volatile u16 *)(anim_s0 + 0x4) = 0x7F;
    *(volatile u16 *)(anim_s0 + 0x6) = 0x224;
}
