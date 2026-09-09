#include "common.h"
typedef struct { s16 vx; s16 vy; s16 vz; s16 pad; } SVECTOR;
typedef struct { s16 m[3][3]; int t[3]; } Matrix;

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void *memset(void *dest, int value, unsigned int count);

extern u8 D_800E2318;
extern u16 D_800E2322;

int func_800C8870(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    u16 *field_s3 = &D_800E2322;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    scale = *(s16 *)(anim + 0x6) + 0x170;
    field = *(u16 *)(anim + 0x4);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    *field_s3 = field;
    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    Gte_ScaleMatrix(&matrix, localScale);
    func_800C42A4((u8 *)field_s3 - 10, &matrix, 1);
}

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void RotMatrix(SVECTOR *rot, Matrix *matrix);
void *memset(void *dest, int value, unsigned int count);

extern SVECTOR D_800C2174;
extern u8 D_800F34D8;
extern u16 D_800F34E2;

int func_800C8970(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    SVECTOR rot;
    int scale_arg[4];
    volatile int scale[4];
    u8 *anim_s0 = anim;
    u16 *field_s1;
    register Matrix *matrix_a0 asm("$4");
    int scale_v0;
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");

        rot = D_800C2174;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    field_s1 = &D_800F34E2;
    *field_s1 = *(u16 *)(anim_s0 + 0x4);
    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim_s0 + 0x8);
    matrix.t[1] = *(s16 *)(anim_s0 + 0xA);
    matrix.t[2] = *(s16 *)(anim_s0 + 0xC);

    memset((void *)scale, 0, sizeof(scale));
    scale[0] = *(s16 *)(anim_s0 + 0x6);
    scale_v0 = *(s16 *)(anim_s0 + 0x6);
    asm("" : : "r"(scale_v0) : "$4");
    matrix_a0 = &matrix;
    asm volatile("" : "=r"(matrix_a0) : "0"(matrix_a0));
    scale[1] = scale_v0;
    scale[2] = *(s16 *)(anim_s0 + 0x6);

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale2_a2 = scale[2];
    scale3_a3 = scale[3];
    scale_arg[2] = scale2_a2;
    scale_arg[3] = scale3_a3;

    asm volatile("" ::: "memory");
    Gte_ScaleMatrix(matrix_a0, scale_arg);
    func_800C42A4((u8 *)field_s1 - 10, &matrix, 0);
}

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void *memset(void *dest, int value, unsigned int count);

extern u8 D_800F34D8;
extern u16 D_800F34E2;

int func_800C8A88(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    int scale_arg[4];
    volatile int scale[4];
    u8 *anim_s0 = anim;
    Matrix *matrix_s1;
    Matrix *call_a0;
    int scale_v0;
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");
    u16 *field_s2 = &D_800F34E2;
    int m0;
    int m1;
    int m2;
    int m3;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field_s2 = *(u16 *)(anim_s0 + 0x4);
    m0 = *(int *)(anim_s0 + 0x10);
    m1 = *(int *)(anim_s0 + 0x14);
    m2 = *(int *)(anim_s0 + 0x18);
    m3 = *(int *)(anim_s0 + 0x1C);
    *(int *)((char *)&matrix + 0x0) = m0;
    *(int *)((char *)&matrix + 0x4) = m1;
    *(int *)((char *)&matrix + 0x8) = m2;
    *(int *)((char *)&matrix + 0xC) = m3;

    m0 = *(int *)(anim_s0 + 0x20);
    m1 = *(int *)(anim_s0 + 0x24);
    m2 = *(int *)(anim_s0 + 0x28);
    m3 = *(int *)(anim_s0 + 0x2C);
    *(int *)((char *)&matrix + 0x10) = m0;
    *(int *)((char *)&matrix + 0x14) = m1;
    *(int *)((char *)&matrix + 0x18) = m2;
    *(int *)((char *)&matrix + 0x1C) = m3;

    matrix.t[0] = *(s16 *)(anim_s0 + 0x8);
    matrix.t[1] = *(s16 *)(anim_s0 + 0xA);
    matrix.t[2] = *(s16 *)(anim_s0 + 0xC);

    memset((void *)scale, 0, sizeof(scale));
    m0 = *(s16 *)(anim_s0 + 0x6);
    matrix_s1 = &matrix;
    scale[0] = m0;
    scale_v0 = *(s16 *)(anim_s0 + 0x6);
    asm("" : : "r"(scale_v0) : "$4");
    call_a0 = matrix_s1;
    asm volatile("" : "=r"(call_a0) : "0"(call_a0));
    scale[1] = scale_v0;
    scale[2] = *(s16 *)(anim_s0 + 0x6);

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale2_a2 = scale[2];
    scale3_a3 = scale[3];
    scale_arg[2] = scale2_a2;
    scale_arg[3] = scale3_a3;

    asm volatile("" ::: "memory");
    Gte_ScaleMatrix(call_a0, scale_arg);
    func_800C42A4((u8 *)field_s2 - 10, matrix_s1, 0);
}

int func_800C8BB4(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C8BC0(void *arg0, u8 *state, u8 *anim) {
    u8 *state_a3 = state;
    u8 *anim_a2 = anim;
    int temp_v0;
    int temp_v1;
    register int temp_a0 asm("$4");
    register int temp_a1 asm("$5");
    u8 count;

    temp_v0 = *(u16 *)(anim_a2 + 0x8);
    temp_v1 = *(u16 *)(anim_a2 + 0x10);
    temp_a0 = *(u16 *)(anim_a2 + 0x12);
    temp_a1 = *(u16 *)(anim_a2 + 0x14);

    temp_v0 += temp_v1;
    *(u16 *)(anim_a2 + 0x8) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0xA);
    temp_v1 = *(u16 *)(anim_a2 + 0xC);
    temp_v0 += temp_a0;
    temp_v1 += temp_a1;
    *(u16 *)(anim_a2 + 0xA) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0x12);
    temp_a0 = (unsigned int)anim_a2;
    *(u16 *)(anim_a2 + 0xC) = temp_v1;
    temp_v1 = anim_a2[1];
    temp_v0 += 3;
    *(u16 *)(anim_a2 + 0x12) = temp_v0;
    temp_v0 = *(s16 *)(anim_a2 + 0xA);
    temp_v1++;
    anim_a2[1] = temp_v1;
    if (temp_v0 > 0) {
        *(u16 *)(anim_a2 + 0x12) = -*(u16 *)(anim_a2 + 0x12);
    }

    count = ((u8 *)temp_a0)[2];
    ((u8 *)temp_a0)[2] = count - 1;
    if (count == 0) {
        state_a3[1] = 2;
    }
}
void func_800C8C4C(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 0x14;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void func_800C8C80(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0x3C;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void func_800C8CBC(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0x78;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void func_800C8CF8(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0xA;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void **FieldEng_GetSlot(void);

extern int D_800E0A50;

extern u8 D_800E22E8;
extern u8 D_800E22E9;
extern u8 D_800E22EA;
extern u8 D_800E22EC;
extern u8 D_800E22ED;
extern u8 D_800E22EE;
extern s16 D_800E22F0;
extern s16 D_800E22F2;

extern u8 D_800F34A8;
extern u8 D_800F34A9;
extern u8 D_800F34AA;
extern u8 D_800F34AC;
extern u8 D_800F34AD;
extern u8 D_800F34AE;
extern s16 D_800F34B0;
extern s16 D_800F34B2;

extern u8 D_800E2328;
extern u8 D_800E2329;
extern u8 D_800E232A;
extern u8 D_800E232C;
extern u8 D_800E232D;
extern u8 D_800E232E;
extern s16 D_800E2330;
extern s16 D_800E2332;

int func_800C8D34(void) {
    int half;
    int shade;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0A50;
    *slot = slotData;

    value = 0xBD;
    D_800E22EC = value;
    value = 9;
    half = 0x80;
    asm volatile("" : "=r"(half) : "0"(half));
    D_800E22ED = value;
    value = 0x80;
    D_800E22E8 = value;
    D_800E22E9 = value;
    D_800E22EA = value;
    value = 0xAE;
    D_800F34AC = value;
    value = 7;
    D_800F34AD = value;
    value = -0x32;
    shade = 0x50;
    asm volatile("" : "=r"(shade) : "0"(shade));
    D_800F34B0 = value;
    D_800E22F0 = 0;
    D_800E22F2 = half;
    D_800E22EE = 0;
    D_800F34B2 = half;
    D_800F34A8 = shade;
    D_800F34A9 = shade;
    D_800F34AA = shade;
    D_800F34AE = 0;
    value = 0x68;
    D_800E232C = value;
    D_800E232D = 0;
    D_800E2330 = 0;
    D_800E2332 = half;
    D_800E2328 = shade;
    D_800E2329 = shade;
    D_800E232A = shade;
    D_800E232E = 0;

    return 0;
}
