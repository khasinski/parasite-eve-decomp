#include "common.h"

int func_800CA4A8(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}

#include "common.h"
int func_800CA4B4(void *arg0, u8 *state, u8 *anim) {
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

#include "common.h"
int func_800CA540(void *arg0, u8 *state, u8 *anim) {
    s16 value = *(u16 *)(anim + 4) - 0x14;

    *(u16 *)(anim + 4) = value;
    if (value < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}

#include "common.h"
void **FieldEng_GetSlot(void);

extern int D_800E0C88;

extern u8 D_800E2308;
extern u8 D_800E2309;
extern u8 D_800E230A;
extern u8 D_800E230C;
extern u8 D_800E230D;
extern u8 D_800E230E;
extern s16 D_800E2310;
extern s16 D_800E2312;

extern u8 D_800E2338;
extern u8 D_800E2339;
extern u8 D_800E233A;
extern u8 D_800E233C;
extern u8 D_800E233D;
extern u8 D_800E233E;
extern s16 D_800E2340;
extern s16 D_800E2342;

extern u8 D_800F34C8;
extern u8 D_800F34C9;
extern u8 D_800F34CA;
extern u8 D_800F34CC;
extern u8 D_800F34CD;
extern u8 D_800F34CE;
extern s16 D_800F34D0;
extern s16 D_800F34D2;

extern u8 D_800F34E8;
extern u8 D_800F34E9;
extern u8 D_800F34EA;
extern u8 D_800F34EC;
extern u8 D_800F34ED;
extern u8 D_800F34EE;
extern s16 D_800F34F0;
extern s16 D_800F34F2;

int func_800CA574(void) {
    int half_a2;
    register int half_a1 asm("$5");
    int shade_a0;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0C88;
    *slot = slotData;

    value = 0xBD;
    D_800E230C = value;
    value = 9;
    half_a2 = 0x80;
    half_a1 = 0x80;
    asm volatile("" : "=r"(half_a2), "=r"(half_a1) : "0"(half_a2), "1"(half_a1));
    D_800E230D = value;
    value = 0xAE;
    D_800F34CC = value;
    value = 7;
    D_800F34CD = value;
    value = -0x32;
    shade_a0 = 0x50;
    asm volatile("" : "=r"(shade_a0) : "0"(shade_a0));
    D_800F34D0 = value;
    value = 0x68;
    D_800E233C = value;
    value = -0x3C;
    D_800E2340 = value;
    value = 0x42;
    D_800F34EC = value;
    value = 0x20;
    D_800F34ED = value;
    value = 0x32;
    D_800E2310 = 0;
    D_800E2312 = half_a2;
    D_800E2308 = half_a1;
    D_800E2309 = half_a1;
    D_800E230A = half_a1;
    D_800E230E = 0;
    D_800F34D2 = half_a2;
    D_800F34C8 = shade_a0;
    D_800F34C9 = shade_a0;
    D_800F34CA = shade_a0;
    D_800F34CE = 0;
    D_800E233D = 0;
    D_800E2342 = half_a2;
    D_800E2338 = shade_a0;
    D_800E2339 = shade_a0;
    D_800E233A = shade_a0;
    D_800E233E = 0;
    D_800F34F0 = value;
    D_800F34F2 = half_a2;
    D_800F34E8 = half_a1;
    D_800F34E9 = half_a1;
    D_800F34EA = half_a1;
    D_800F34EE = 0;

    return 0;
}

extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800CA6D4(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

extern char D_800E0B84[];
extern void FieldEng_Register(void *o, void *table);

int func_800CA700(void *o) {
    FieldEng_Register(o, D_800E0B84);
    return 0;
}

extern char D_800E0BA0[];
extern char D_800E0B68[];
extern char D_800E0BBC[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CA798(char *obj);

int func_800CA728(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0BA0);
    second = func_800C2758(obj, D_800E0B68, D_800E0BBC);
    status = first | second;
    if (status == -1) {
        func_800CA798(obj);
    }
    return 0;
}

int func_800CA798(char *arg0) {
    *arg0 = 4;
    return 0;
}

int func_800CA7A8(void) {
    return 0;
}

int func_800CA7B0(void) {
    return 0;
}

#include "common.h"
void func_800CEDA8(int arg0);

extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;
extern char *D_800E27A8;

int func_800CA7B8(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2360 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E2362 = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E2364 = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A8 = data;
    func_800CEDA8(0);
}

#include "common.h"
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

int rand(void);
void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);

extern char *D_800E27A8;
extern u16 D_800E2360;
extern volatile u16 D_800E2362;
extern u16 D_800E2364;

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

#include "common.h"


void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);

extern char *D_8009D254;
extern char D_800E0C08[];
extern char *D_800E27A8;
extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;

int func_800CA934(void *arg0, void *arg1, u8 *anim) {
    u8 *anim_s0 = anim;
    SVECTOR out;
    char *data;
    char *entry;
    char *model;
    int m0;
    int m1;
    int m2;
    int m3;

    entry = D_800E0C08 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, entry, &out);

    *(u16 *)(anim_s0 + 0x8) = D_800E2360 + out.vx;
    *(u16 *)(anim_s0 + 0xA) = D_800E2362 + out.vy;
    data = D_800E27A8;
    *(u16 *)(anim_s0 + 0xC) = D_800E2364 + out.vz;

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

#include "common.h"


void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);

extern SVECTOR D_800C21C4;
extern SVECTOR D_800C21CC;
extern char *D_800E27A8;
extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;

int func_800CAA38(void *arg0, void *arg1, u8 *anim) {
    u8 *anim_s0;
    u16 *field_s1;
    SVECTOR *out_s2;
    register int y_base_v0 asm("$2");
    register u16 y_out_v1 asm("$3");
    register int z_base_v1 asm("$3");
    u16 z_out_a0;
    SVECTOR in0;
    SVECTOR in1;
    SVECTOR out;
    char *data;

    anim_s0 = anim;

    in0 = D_800C21C4;
    in1 = D_800C21CC;
    out_s2 = &out;

    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, &in0, out_s2);
    field_s1 = &D_800E2360;
    *(u16 *)(anim_s0 + 0x8) = field_s1[0] + out.vx;
    *(u16 *)(anim_s0 + 0xA) = D_800E2362 + out.vy;
    data = D_800E27A8;
    *(u16 *)(anim_s0 + 0xC) = D_800E2364 + out.vz;

    ApplyMatrixSV(*(char **)(data + 0x238) + 0x260, &in1, out_s2);
    *(u16 *)(anim_s0 + 0x10) = field_s1[0] + out.vx;
    y_base_v0 = D_800E2362;
    asm volatile("" : "=r"(y_base_v0) : "0"(y_base_v0));
    y_out_v1 = out.vy;
    *(u16 *)(anim_s0 + 0x12) = y_base_v0 + y_out_v1;
    z_base_v1 = D_800E2364;
    z_out_a0 = out.vz;
    *(u16 *)(anim_s0 + 0x4) = 0x7F;
    *(u16 *)(anim_s0 + 0x6) = 0;
    *(u16 *)(anim_s0 + 0x14) = z_base_v1 + z_out_a0;
}

#include "common.h"
extern char *volatile D_8009D254;
extern u16 D_800E2360;
extern u16 D_800E2364;

int func_800CAB88(void *arg0, void *arg1, u8 *anim) {
    u16 x = D_800E2360;
    char *data = D_8009D254;
    int y;
    u16 z;

    *(u16 *)(anim + 0x8) = x;
    y = *(s16 *)(data + 0x2E);
    *(u16 *)(anim + 0xA) = y;
    z = D_800E2364;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x224;
    *(u16 *)(anim + 0xC) = z;
}

#include "common.h"


void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);

extern char *D_8009D254;
extern char D_800E0C48[];
extern char *D_800E27A8;
extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;

int func_800CABC8(void *arg0, void *arg1, u8 *anim) {
    u8 *anim_s0 = anim;
    SVECTOR out;
    char *data;
    char *entry;
    char *model;
    int m0;
    int m1;
    int m2;
    int m3;

    entry = D_800E0C48 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, entry, &out);

    *(u16 *)(anim_s0 + 0x8) = D_800E2360 + out.vx;
    *(u16 *)(anim_s0 + 0xA) = D_800E2362 + out.vy;
    data = D_800E27A8;
    *(u16 *)(anim_s0 + 0xC) = D_800E2364 + out.vz;

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
    *(u16 *)(anim_s0 + 0x6) = 0x224;
}

void func_800CACD4(void) {
}

#include "common.h"


typedef struct {
    s16 m[3][3];
    int t[3];
} Matrix;

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void RotMatrix(SVECTOR *rot, Matrix *matrix);
void *memset(void *dest, int value, unsigned int count);

extern char *D_8009D254;
extern s16 D_800E0BE8[];
extern u8 D_800E2308;

int func_800CACDC(void *arg0, void *arg1, u8 *anim) {
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
    scale[0] = D_800E0BE8[index_s0 >> 1];
    scale_v0 = D_800E0BE8[index_s0 >> 1];
    asm("" : : "r"(scale_v0) : "$4");
    matrix_a0 = &matrix;
    asm volatile("" : "=r"(matrix_a0) : "0"(matrix_a0));
    scale[1] = scale_v0;
    scale[2] = D_800E0BE8[index_s0 >> 1];

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale2_a2 = scale[2];
    scale3_a3 = scale[3];
    scale_arg[2] = scale2_a2;
    scale_arg[3] = scale3_a3;

    asm volatile("" ::: "memory");
    Gte_ScaleMatrix(matrix_a0, scale_arg);
    func_800C42A4(&D_800E2308, &matrix, 1);
}
