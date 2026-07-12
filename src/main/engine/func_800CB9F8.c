typedef unsigned char u8;
typedef signed short s16;

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
void *memset(void *dest, int value, unsigned int count);

extern u8 D_800F34E8;
extern s16 D_800F34F2;

int func_800CB9F8(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    int scale[4];

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800F34F2 = *(unsigned short *)(anim + 0x4);

    *(int *)((char *)&matrix + 0x0) = *(int *)(anim + 0x10);
    *(int *)((char *)&matrix + 0x4) = *(int *)(anim + 0x14);
    *(int *)((char *)&matrix + 0x8) = *(int *)(anim + 0x18);
    *(int *)((char *)&matrix + 0xC) = *(int *)(anim + 0x1C);
    *(int *)((char *)&matrix + 0x10) = *(int *)(anim + 0x20);
    *(int *)((char *)&matrix + 0x14) = *(int *)(anim + 0x24);
    *(int *)((char *)&matrix + 0x18) = *(int *)(anim + 0x28);
    *(int *)((char *)&matrix + 0x1C) = *(int *)(anim + 0x2C);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(scale, 0, sizeof(scale));
    scale[0] = *(s16 *)(anim + 0x6);
    scale[1] = *(s16 *)(anim + 0x6);
    scale[2] = *(s16 *)(anim + 0x6);

    Gte_ScaleMatrix(&matrix, scale);
    func_800C42A4(&D_800F34E8, &matrix, 0);
}
