typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    short vx;
    short vy;
    short vz;
    short pad;
} SVECTOR;

void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);

extern SVECTOR D_800C214C;
extern char *D_800E279C;
extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;

int func_800C8064(void *arg0, void *arg1, u8 *anim) {
    SVECTOR in;
    SVECTOR out;
    char *model;

    in = D_800C214C;

    model = *(char **)(D_800E279C + 0x238);
    ApplyMatrixSV(model + 0x260, &in, &out);

    *(u16 *)(anim + 0x8) = D_800E2348 + out.vx;
    *(u16 *)(anim + 0xA) = D_800E234A + out.vy;
    *(u16 *)(anim + 0xC) = D_800E234C + out.vz;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0;
}
