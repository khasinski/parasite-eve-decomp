#include "common.h"
extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800C8E44(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
extern char D_800E09A0[];
extern void FieldEng_Register(void *o, void *table);

int func_800C8E70(void *o) {
    FieldEng_Register(o, D_800E09A0);
    return 0;
}
extern char D_800E09B4[];
extern char D_800E098C[];
extern char D_800E09C8[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C8F08(char *obj);

int func_800C8E98(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E09B4);
    second = func_800C2758(obj, D_800E098C, D_800E09C8);
    status = first | second;
    if (status == -1) {
        func_800C8F08(obj);
    }
    return 0;
}
int func_800C8F08(char *arg0) {
    *arg0 = 4;
    return 0;
}
int func_800C8F18(void) {
    return 0;
}
int func_800C8F20(void) {
    return 0;
}
void func_800CEDA8(int arg0);

extern u16 D_800E2350;
extern u16 D_800E2352;
extern u16 D_800E2354;
extern char *D_800E27A0;

int func_800C8F28(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2350 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E2352 = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E2354 = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A0 = data;
    func_800CEDA8(0);
}
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
