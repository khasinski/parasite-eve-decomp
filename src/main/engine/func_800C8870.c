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

extern u8 D_800E2318;
extern u16 D_800E2322;

int func_800C8870(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    int scale;
    int localScale[4];

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    matrix.m[0][0] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[2][2] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    D_800E2322 = *(u16 *)(anim + 0x4);
    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    scale = *(s16 *)(anim + 0x6) + 0x170;
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    Gte_ScaleMatrix(&matrix, localScale);
    func_800C42A4(&D_800E2318, &matrix, 1);
}
