typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

void ApplyMatrixSV(void *matrix, SVECTOR *in, void *out);
int rand(void);

extern char *D_800E279C;
extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;

int func_800C7E50(void *arg0, void *arg1, u8 *anim) {
    SVECTOR vec;
    int (*model)[];

    vec.vx = -(rand() % 3 + 9);
    vec.vy = -(rand() % 3 + 9);
    vec.vz = rand() % 5 - 2;

    model = *(int (**)[])(D_800E279C + 0x238);
    ApplyMatrixSV(model, &vec, anim + 0x10);

    *(u16 *)(anim + 0x8) = D_800E2348;
    *(u16 *)(anim + 0xA) = D_800E234A;
    *(u16 *)(anim + 0xC) = D_800E234C;
    anim[2] = 0x14;
    anim[1] = 0;
}
