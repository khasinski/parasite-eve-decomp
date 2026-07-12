typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    short vx;
    short vy;
    short vz;
    short pad;
} SVECTOR;

void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);

extern SVECTOR D_800C21C4;
extern SVECTOR D_800C21CC;
extern char *D_800E27A8;
extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;

int func_800CAA38(void *arg0, void *arg1, u8 *anim) {
    register u8 *anim_s0 asm("$16") = anim;
    register u16 *field_s1 asm("$17") = &D_800E2360;
    register SVECTOR *out_s2 asm("$18");
    SVECTOR in0;
    SVECTOR in1;
    SVECTOR out;
    char *data;

    in0 = D_800C21C4;
    in1 = D_800C21CC;
    out_s2 = &out;

    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, &in0, out_s2);
    *(u16 *)(anim_s0 + 0x8) = field_s1[0] + out.vx;
    *(u16 *)(anim_s0 + 0xA) = D_800E2362 + out.vy;
    data = D_800E27A8;
    *(u16 *)(anim_s0 + 0xC) = D_800E2364 + out.vz;

    ApplyMatrixSV(*(char **)(data + 0x238) + 0x260, &in1, out_s2);
    *(u16 *)(anim_s0 + 0x10) = field_s1[0] + out.vx;
    *(u16 *)(anim_s0 + 0x12) = D_800E2362 + out.vy;
    *(u16 *)(anim_s0 + 0x4) = 0x7F;
    *(u16 *)(anim_s0 + 0x6) = 0;
    *(u16 *)(anim_s0 + 0x14) = D_800E2364 + out.vz;
}
