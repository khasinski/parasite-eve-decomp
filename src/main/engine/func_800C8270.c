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

extern char *D_8009D254;
extern s16 D_800E0888[];
extern u8 D_800E22D8;

int func_800C8270(void *arg0, void *arg1, u8 *anim) {
    Matrix matrix;
    int position[3];
    SVECTOR rot;
    int scale[4];
    s16 value;

    value = D_800E0888[*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1];

    rot.vx = 0;
    rot.vy = 0;
    rot.vz = anim[1] << 6;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    RotMatrix(&rot, &matrix);

    position[0] = *(s16 *)(anim + 0x8);
    position[1] = *(s16 *)(anim + 0xA);
    position[2] = *(s16 *)(anim + 0xC);

    memset(scale, 0, sizeof(scale));
    scale[0] = value;
    scale[1] = value;
    scale[2] = value;

    matrix.t[0] = position[0];
    matrix.t[1] = position[1];
    matrix.t[2] = position[2];

    Gte_ScaleMatrix(&matrix, scale);
    func_800C42A4(&D_800E22D8, &matrix, 1);
}
