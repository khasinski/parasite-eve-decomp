#include "common.h"
typedef struct { s16 m[3][3]; int t[3]; } Matrix;

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void *memset(void *dest, int value, unsigned int count);

extern u8 D_800E2328;
extern u16 D_800E2332;

int func_800C9868(void *arg0, void *arg1, u8 *anim) {
    u16 *field_s3 = &D_800E2332;
    Matrix matrix;
    int scale;
    u16 field;
    int localScale[4];

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

int func_800C9968(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C9974(void *arg0, u8 *state, u8 *anim) {
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
int func_800C9A00(void *arg0, u8 *state, u8 *anim) {
    s16 value = *(u16 *)(anim + 4) - 0x14;

    *(u16 *)(anim + 4) = value;
    if (value < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}
int func_800C9A34(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) - 8;
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6) + 0x28;
    if (*(s16 *)(anim + 4) < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}
void **FieldEng_GetSlot(void);

extern int D_800E0B38;

extern u8 D_800E22F8;
extern u8 D_800E22F9;
extern u8 D_800E22FA;
extern u8 D_800E22FC;
extern u8 D_800E22FD;
extern u8 D_800E22FE;
extern s16 D_800E2300;
extern s16 D_800E2302;

extern u8 D_800F34B8;
extern u8 D_800F34B9;
extern u8 D_800F34BA;
extern u8 D_800F34BC;
extern u8 D_800F34BD;
extern u8 D_800F34BE;
extern s16 D_800F34C0;
extern s16 D_800F34C2;

int func_800C9A70(void) {
    int half;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0B38;
    *slot = slotData;

    value = 0xBD;
    D_800E22FC = value;
    value = 9;
    half = 0x80;
    asm volatile("" : "=r"(half) : "0"(half));
    D_800E22FD = value;
    value = 0x80;
    D_800E22F8 = value;
    D_800E22F9 = value;
    D_800E22FA = value;
    value = 0xAC;
    D_800F34BC = value;
    value = 6;
    D_800F34BD = value;
    value = -0x32;
    D_800F34C0 = value;
    value = 0x50;
    D_800E2300 = 0;
    D_800E2302 = half;
    D_800E22FE = 0;
    D_800F34C2 = half;
    D_800F34B8 = value;
    D_800F34B9 = value;
    D_800F34BA = value;
    D_800F34BE = 0;

    return 0;
}
extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800C9B3C(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
extern char D_800E0A94[];
extern void FieldEng_Register(void *o, void *table);

int func_800C9B68(void *o) {
    FieldEng_Register(o, D_800E0A94);
    return 0;
}
extern char D_800E0AA4[];
extern char D_800E0A84[];
extern char D_800E0AB4[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C9C00(char *obj);

int func_800C9B90(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0AA4);
    second = func_800C2758(obj, D_800E0A84, D_800E0AB4);
    status = first | second;
    if (status == -1) {
        func_800C9C00(obj);
    }
    return 0;
}
int func_800C9C00(char *arg0) {
    *arg0 = 4;
    return 0;
}
int func_800C9C10(void) {
    return 0;
}
int func_800C9C18(void) {
    return 0;
}
void func_800CEDA8(int arg0);

extern u16 D_800E2358;
extern u16 D_800E235A;
extern u16 D_800E235C;
extern char *D_800E27A4;

int func_800C9C20(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2358 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E235A = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E235C = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A4 = data;
    func_800CEDA8(0);
}


typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

void ApplyMatrixSV();
int rand(void);

extern char *D_800E27A4;
extern u16 D_800E2358;
extern u16 D_800E235A;
extern u16 D_800E235C;

int func_800C9C8C(void *arg0, void *arg1, u8 *anim) {
    SVECTOR vec;
    int (*model)[];

    vec.vx = -(rand() % 3 + 9);
    vec.vy = -(rand() % 3 + 9);
    vec.vz = rand() % 5 - 2;

    model = *(int (**)[])(D_800E27A4 + 0x238);
    ApplyMatrixSV(model, &vec, anim + 0x10);

    *(u16 *)(anim + 0x8) = D_800E2358;
    *(u16 *)(anim + 0xA) = D_800E235A;
    *(u16 *)(anim + 0xC) = D_800E235C;
    anim[2] = 0x14;
    anim[1] = 0;
}



extern char *D_8009D254;
extern char D_800E0AF8[];
extern char *D_800E27A4;
extern u16 D_800E2358;
extern u16 D_800E235A;
extern u16 D_800E235C;

int func_800C9D9C(void *arg0, void *arg1, u8 *anim) {
    u8 *anim_s0 = anim;
    SVECTOR out;
    char *data;
    char *entry;
    char *model;
    int m0;
    int m1;
    int m2;
    int m3;

    entry = D_800E0AF8 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A4 + 0x238) + 0x260, entry, &out);

    *(u16 *)(anim_s0 + 0x8) = D_800E2358 + out.vx;
    *(u16 *)(anim_s0 + 0xA) = D_800E235A + out.vy;
    data = D_800E27A4;
    *(u16 *)(anim_s0 + 0xC) = D_800E235C + out.vz;

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

void func_800C9EA0(void) {
}


void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void RotMatrix(SVECTOR *rot, Matrix *matrix);
void *memset(void *dest, int value, unsigned int count);

extern char *D_8009D254;
extern s16 D_800E0AD8[];
extern u8 D_800E22F8;

int func_800C9EA8(void *arg0, void *arg1, u8 *anim) {
    register int index_s0 asm("$16");
    u8 *anim_s1 = anim;
    int rot_v0;
    register Matrix *matrix_a0 asm("$4");
    int scale_v0;
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");
    Matrix matrix;
    SVECTOR rot;
    int scale_arg[4];
    volatile int scale[4];

    index_s0 = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6);

    rot.vx = 0;
    rot.vy = 0;
    rot_v0 = anim_s1[1];
    rot_v0 <<= 24;
    rot_v0 >>= 18;
    index_s0--;
    rot.vz = rot_v0;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim_s1 + 0x8);
    matrix.t[1] = *(s16 *)(anim_s1 + 0xA);
    matrix.t[2] = *(s16 *)(anim_s1 + 0xC);

    memset((void *)scale, 0, sizeof(scale));
    index_s0 <<= 16;
    index_s0 >>= 15;
    scale[0] = D_800E0AD8[index_s0 >> 1];
    scale_v0 = D_800E0AD8[index_s0 >> 1];
    asm("" : : "r"(scale_v0) : "$4");
    matrix_a0 = &matrix;
    asm volatile("" : "=r"(matrix_a0) : "0"(matrix_a0));
    scale[1] = scale_v0;
    scale[2] = D_800E0AD8[index_s0 >> 1];

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale2_a2 = scale[2];
    scale3_a3 = scale[3];
    scale_arg[2] = scale2_a2;
    scale_arg[3] = scale3_a3;

    asm volatile("" ::: "memory");
    Gte_ScaleMatrix(matrix_a0, scale_arg);
    func_800C42A4(&D_800E22F8, &matrix, 1);
}
