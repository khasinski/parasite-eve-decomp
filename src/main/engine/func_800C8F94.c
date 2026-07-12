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

extern char *D_800E27A0;
extern volatile u16 D_800E2350;
extern volatile u16 D_800E2352;
extern volatile u16 D_800E2354;

int func_800C8F94(void *arg0, void *arg1, u8 *anim) {
    SVECTOR vec;

    vec.vx = -((rand() % 3) + 9);
    vec.vy = -((rand() % 3) + 9);
    vec.vz = (rand() % 5) - 2;

    ApplyMatrixSV(*(void **)(D_800E27A0 + 0x238), &vec, anim + 0x10);

    {
        u16 z;

        *(u16 *)(anim + 0x8) = D_800E2350;
        *(u16 *)(anim + 0xA) = D_800E2352;
        z = D_800E2354;
        anim[2] = 0x14;
        anim[1] = 0;
        *(u16 *)(anim + 0xC) = z;
    }
}
