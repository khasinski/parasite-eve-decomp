typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

typedef struct { s16 vx; s16 vy; s16 vz; s16 pad; } SVECTOR;
typedef struct { s16 m[3][3]; int t[3]; } Matrix;

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
    register u8 *anim_s0 asm("$16") = anim;
    register u16 *field_s1 asm("$17");
    register Matrix *matrix_a0 asm("$4");
    register int scale_v0 asm("$2");
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");

    asm volatile("" : "=r"(anim_s0) : "0"(anim_s0));
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
    asm volatile("addiu %0, $sp, 0x10" : "=r"(matrix_a0));
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
