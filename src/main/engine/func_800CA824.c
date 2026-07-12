typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

int rand(void);
void ApplyMatrixSV(void *matrix, SVECTOR *in, void *out);

extern char *D_800E27A8;
extern volatile u16 D_800E2360;
extern volatile u16 D_800E2362;
extern volatile u16 D_800E2364;

int func_800CA824(void *arg0, void *arg1, u8 *anim) {
    SVECTOR vec;

    vec.vx = -((rand() % 3) + 9);
    vec.vy = -((rand() % 3) + 9);
    vec.vz = (rand() % 5) - 2;

    ApplyMatrixSV(*(void **)(D_800E27A8 + 0x238), &vec, anim + 0x10);

    {
        u16 z;

        *(u16 *)(anim + 0x8) = D_800E2360;
        *(u16 *)(anim + 0xA) = D_800E2362;
        z = D_800E2364;
        anim[2] = 0x14;
        anim[1] = 0;
        *(u16 *)(anim + 0xC) = z;
    }
}
