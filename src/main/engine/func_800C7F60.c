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
extern char D_800E08A8[];
extern char *D_800E279C;
extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;

int func_800C7F60(void *arg0, void *arg1, u8 *anim) {
    SVECTOR out;
    char *model;
    char *entry;

    entry = D_800E08A8 + ((*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1) << 3);
    model = *(char **)(D_800E279C + 0x238);
    ApplyMatrixSV(model + 0x260, entry, &out);

    *(u16 *)(anim + 0x8) = D_800E2348 + out.vx;
    *(u16 *)(anim + 0xA) = D_800E234A + out.vy;
    *(u16 *)(anim + 0xC) = D_800E234C + out.vz;

    *(int *)(anim + 0x10) = *(int *)(model + 0x260);
    *(int *)(anim + 0x14) = *(int *)(model + 0x264);
    *(int *)(anim + 0x18) = *(int *)(model + 0x268);
    *(int *)(anim + 0x1C) = *(int *)(model + 0x26C);
    *(int *)(anim + 0x20) = *(int *)(model + 0x270);
    *(int *)(anim + 0x24) = *(int *)(model + 0x274);
    *(int *)(anim + 0x28) = *(int *)(model + 0x278);
    *(int *)(anim + 0x2C) = *(int *)(model + 0x27C);
    *(u16 *)(anim + 0x4) = 0x7F;
}
