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
    SVECTOR in0;
    SVECTOR in1;
    SVECTOR out;
    char *model;

    in0 = D_800C21C4;
    in1 = D_800C21CC;
    model = *(char **)(D_800E27A8 + 0x238);

    ApplyMatrixSV(model + 0x260, &in0, &out);
    *(u16 *)(anim + 0x8) = D_800E2360 + out.vx;
    *(u16 *)(anim + 0xA) = D_800E2362 + out.vy;
    *(u16 *)(anim + 0xC) = D_800E2364 + out.vz;

    ApplyMatrixSV(model + 0x260, &in1, &out);
    *(u16 *)(anim + 0x10) = D_800E2360 + out.vx;
    *(u16 *)(anim + 0x12) = D_800E2362 + out.vy;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0;
    *(u16 *)(anim + 0x14) = D_800E2364 + out.vz;
}
