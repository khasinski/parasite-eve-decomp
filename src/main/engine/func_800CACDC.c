typedef unsigned char u8;
typedef unsigned short u16;
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

extern char *D_8009D254;
extern s16 D_800E0BE8[];
extern u8 D_800E2308;

int func_800CACDC(void *arg0, void *arg1, u8 *anim) {
    register int index_s0 asm("$16");
    register u8 *anim_s1 asm("$17") = anim;
    register Matrix *matrix_a0 asm("$4");
    Matrix matrix;
    SVECTOR rot;
    int scale_arg[4];
    volatile int scale[4];

    index_s0 = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6);

    rot.vx = 0;
    rot.vy = 0;
    rot.vz = ((signed char)anim_s1[1]) << 6;

    index_s0--;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim_s1 + 0x8);
    matrix.t[1] = *(s16 *)(anim_s1 + 0xA);
    matrix.t[2] = *(s16 *)(anim_s1 + 0xC);

    memset(scale, 0, sizeof(scale));
    index_s0 = (int)&D_800E0BE8[(short)index_s0];
    scale[0] = *(s16 *)index_s0;
    scale[1] = *(s16 *)index_s0;
    matrix_a0 = &matrix;
    asm volatile("" : "=r"(matrix_a0) : "0"(matrix_a0));
    scale[2] = *(s16 *)index_s0;
    asm volatile("nop");

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale_arg[2] = scale[2];
    scale_arg[3] = scale[3];

    Gte_ScaleMatrix(matrix_a0, scale_arg);
    func_800C42A4(&D_800E2308, &matrix, 1);
}
