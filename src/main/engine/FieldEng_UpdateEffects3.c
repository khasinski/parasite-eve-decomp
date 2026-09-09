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
