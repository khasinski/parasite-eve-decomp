#include "common.h"
#include "pe1/gte_types.h"
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

int rand(void);

extern char *D_800E27A0;
extern volatile u16 D_800E2350;
extern volatile u16 D_800E2352;
extern volatile u16 D_800E2354;

int func_800C8F94(void *arg0, void *arg1, u8 *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

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

extern char *D_8009D254;
extern char D_800E0A10[];
extern char *D_800E27A0;
extern u16 D_800E2350;
extern u16 D_800E2352;
extern u16 D_800E2354;

int func_800C90A4(void *arg0, void *arg1, u8 *anim) {
    u8 *anim_s0 = anim;
    GteShortVector out;
    char *data;
    char *entry;
    char *model;
    int m0;
    int m1;
    int m2;
    int m3;

    entry = D_800E0A10 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A0 + 0x238) + 0x260, entry, &out);

    *(u16 *)(anim_s0 + 0x8) = D_800E2350 + out.x;
    *(u16 *)(anim_s0 + 0xA) = D_800E2352 + out.y;
    data = D_800E27A0;
    *(u16 *)(anim_s0 + 0xC) = D_800E2354 + out.z;

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
    *(u16 *)(anim_s0 + 0x4) = 0x7F;
}

extern GteShortVector D_800C217C;
extern char *D_800E27A0;
extern volatile u16 D_800E2350;
extern volatile u16 D_800E2352;
extern volatile u16 D_800E2354;

int func_800C91A8(void *arg0, void *arg1, u8 *anim) {
    GteShortVector in;
    GteShortVector out;

    in = D_800C217C;
    ApplyMatrixSV(*(char **)(D_800E27A0 + 0x238) + 0x260, &in, &out);

    {
        u16 y;
        u16 z;

        *(u16 *)(anim + 0x8) = D_800E2350 + out.x;
        y = D_800E2352 + out.y;
        *(u16 *)(anim + 0xA) = y;
        z = D_800E2354 + out.z;
        *(u16 *)(anim + 0x4) = 0x7F;
        *(u16 *)(anim + 0x6) = 0;
        *(u16 *)(anim + 0xC) = z;
    }
}

void func_800C9260(void) {
}

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void *memset(void *dest, int value, unsigned int count);

extern char *D_8009D254;
extern s16 D_800E09F0[];
extern u8 D_800E22E8;

int func_800C9268(void *arg0, void *arg1, u8 *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim[1] << 6;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(&scale, 0, sizeof(scale));
    scale.x = D_800E09F0[index];
    scale.y = D_800E09F0[index];
    scale.z = D_800E09F0[index];
    scaleCopy = scale;

    Gte_ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22E8, &matrix, 1);
}
