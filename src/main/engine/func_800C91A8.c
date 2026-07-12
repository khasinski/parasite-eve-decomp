typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    short vx;
    short vy;
    short vz;
    short pad;
} SVECTOR;

void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);

extern SVECTOR D_800C217C;
extern char *D_800E27A0;
extern u16 D_800E2350;
extern u16 D_800E2352;
extern u16 D_800E2354;

int func_800C91A8(void *arg0, void *arg1, u8 *anim) {
    SVECTOR in;
    SVECTOR out;

    in = D_800C217C;
    ApplyMatrixSV(*(char **)(D_800E27A0 + 0x238) + 0x260, &in, &out);

    *(u16 *)(anim + 0x8) = D_800E2350 + out.vx;
    *(u16 *)(anim + 0xA) = D_800E2352 + out.vy;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0;
    *(u16 *)(anim + 0xC) = D_800E2354 + out.vz;
}
