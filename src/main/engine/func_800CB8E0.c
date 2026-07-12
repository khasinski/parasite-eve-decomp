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

extern SVECTOR D_800C2204;
extern u8 D_800F34E8;
extern s16 D_800F34F2;

int func_800CB8E0(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    int position[3];
    SVECTOR rot;
    int scale[4];

    rot = D_800C2204;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800F34F2 = *(unsigned short *)(anim + 0x4);
    RotMatrix(&rot, &matrix);

    position[0] = *(s16 *)(anim + 0x8);
    position[1] = *(s16 *)(anim + 0xA);
    position[2] = *(s16 *)(anim + 0xC);

    memset(scale, 0, sizeof(scale));
    scale[0] = *(s16 *)(anim + 0x6);
    scale[1] = *(s16 *)(anim + 0x6);
    scale[2] = *(s16 *)(anim + 0x6);

    matrix.t[0] = position[0];
    matrix.t[1] = position[1];
    matrix.t[2] = position[2];

    Gte_ScaleMatrix(&matrix, scale);
    func_800C42A4(&D_800F34E8, &matrix, 0);
}
