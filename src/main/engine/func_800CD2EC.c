typedef unsigned char u8;
typedef signed short s16;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

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

extern SVECTOR D_800C223C;
extern u8 D_800E2298;
extern s16 D_800E22A2;

int func_800CD2EC(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    SVECTOR rot;
    int scale_arg[4];
    volatile int scale[4];
    register s16 *field_s1 asm("$17") = &D_800E22A2;
    register Matrix *matrix_a0 asm("$4");
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");

    rot = D_800C223C;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field_s1 = *(unsigned short *)(anim + 0x4);
    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset((void *)scale, 0, sizeof(scale));
    scale[0] = *(s16 *)(anim + 0x6);
    scale[1] = *(s16 *)(anim + 0x6);
    matrix_a0 = &matrix;
    asm volatile("" : "=r"(matrix_a0) : "0"(matrix_a0));
    scale[2] = *(s16 *)(anim + 0x6);

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale2_a2 = scale[2];
    scale3_a3 = scale[3];
    scale_arg[2] = scale2_a2;
    scale_arg[3] = scale3_a3;
    asm volatile("" ::: "$5");

    Gte_ScaleMatrix(matrix_a0, scale_arg);
    func_800C42A4((u8 *)field_s1 - 0xA, &matrix, 0);
}
