typedef unsigned char u8;
typedef signed short s16;

typedef struct {
    s16 m[3][3];
    int t[3];
} Matrix;

typedef struct {
    int v[4];
} Scale;

void **FieldEng_GetSlot(void);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);

extern int D_800C220C[];
extern int D_800C221C[];
extern int D_800C222C[];
extern int D_800E0EB8;

extern Matrix D_800F3478;
extern Matrix D_800F33C0;
extern Matrix D_800F32B0;

extern u8 D_800E2298;
extern u8 D_800E2299;
extern u8 D_800E229A;
extern u8 D_800E229C;
extern u8 D_800E229D;
extern s16 D_800E22A0;
extern s16 D_800E22A2;

extern u8 D_800E2250;
extern u8 D_800E2251;
extern u8 D_800E2252;
extern u8 D_800E2254;
extern u8 D_800E2255;
extern s16 D_800E2258;
extern s16 D_800E225A;

extern u8 D_800E27E0;
extern u8 D_800E27E1;
extern u8 D_800E27E2;
extern u8 D_800E27E4;
extern u8 D_800E27E5;
extern s16 D_800E27E8;
extern s16 D_800E27EA;

extern u8 D_800F3460;
extern u8 D_800F3461;
extern u8 D_800F3462;
extern u8 D_800F3464;
extern u8 D_800F3465;
extern s16 D_800F3468;
extern s16 D_800F346A;

void func_800CCBA8(void) {
    int scale0[4];
    int scale1[4];
    int scale2[4];
    int one = 0x1000;
    int neg100;
    int full;
    int half;

    *FieldEng_GetSlot() = &D_800E0EB8;

    *(Scale *)scale0 = *(Scale *)D_800C220C;
    D_800F3478.m[2][2] = one;
    D_800F3478.m[1][1] = one;
    D_800F3478.m[0][0] = one;
    D_800F3478.t[2] = 0;
    D_800F3478.t[1] = 0;
    D_800F3478.t[0] = 0;
    D_800F3478.m[2][1] = 0;
    D_800F3478.m[2][0] = 0;
    D_800F3478.m[1][2] = 0;
    D_800F3478.m[1][0] = 0;
    D_800F3478.m[0][2] = 0;
    D_800F3478.m[0][1] = 0;
    Gte_ScaleMatrix(&D_800F3478, scale0);

    *(Scale *)scale1 = *(Scale *)D_800C221C;
    D_800F33C0.m[2][2] = one;
    D_800F33C0.m[1][1] = one;
    D_800F33C0.m[0][0] = one;
    D_800F33C0.t[2] = 0;
    D_800F33C0.t[1] = 0;
    D_800F33C0.t[0] = 0;
    D_800F33C0.m[2][1] = 0;
    D_800F33C0.m[2][0] = 0;
    D_800F33C0.m[1][2] = 0;
    D_800F33C0.m[1][0] = 0;
    D_800F33C0.m[0][2] = 0;
    D_800F33C0.m[0][1] = 0;
    Gte_ScaleMatrix(&D_800F33C0, scale1);

    *(Scale *)scale2 = *(Scale *)D_800C222C;
    D_800F32B0.m[2][2] = one;
    D_800F32B0.m[1][1] = one;
    D_800F32B0.m[0][0] = one;
    D_800F32B0.t[2] = 0;
    D_800F32B0.t[1] = 0;
    D_800F32B0.t[0] = 0;
    D_800F32B0.m[2][1] = 0;
    D_800F32B0.m[2][0] = 0;
    D_800F32B0.m[1][2] = 0;
    D_800F32B0.m[1][0] = 0;
    D_800F32B0.m[0][2] = 0;
    D_800F32B0.m[0][1] = 0;
    Gte_ScaleMatrix(&D_800F32B0, scale2);

    asm volatile("move $2,$0");
    D_800E229C = 0x42;
    D_800E229D = 0x20;
    D_800E22A0 = 0x32;
    D_800E2255 = 4;
    D_800E2258 = -0x32;
    D_800E27E4 = 0x68;
    D_800E27E8 = -0x63;
    D_800E27E0 = 0x40;
    D_800E27E2 = 0x40;
    D_800F3464 = 0x6E;
    D_800F3465 = 3;

    neg100 = -0x64;
    half = 0x7F;
    full = 0x80;

    D_800E22A2 = half;
    D_800E2298 = full;
    D_800E2299 = full;
    D_800E229A = full;
    D_800E2254 = full;
    D_800E225A = half;
    D_800E2250 = full;
    D_800E2251 = full;
    D_800E2252 = full;
    D_800E27E5 = 0;
    D_800E27EA = half;
    D_800E27E1 = full;
    D_800F3468 = neg100;
    D_800F346A = half;
    D_800F3460 = full;
    D_800F3461 = full;
    D_800F3462 = full;
}
