typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

typedef struct { s16 m[3][3]; int t[3]; } Matrix;

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
    register Matrix *matrix_s1 asm("$17");
    register Matrix *call_a0 asm("$4");
    register int scale_v0 asm("$2");
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");
    register u16 *field_s2 asm("$18") = &D_800F34E2;
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
    asm volatile("addiu %0, $sp, 0x10" : "=r"(matrix_s1));
    scale[0] = m0;
    scale_v0 = *(s16 *)(anim_s0 + 0x6);
    asm volatile("addu %0, %1, $zero" : "=r"(call_a0) : "r"(matrix_s1));
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
