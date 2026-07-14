typedef unsigned char u8;
typedef signed short s16;
typedef int s32;

typedef struct Room328Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Room328Matrix;

typedef struct Room328Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Room328Vec4;

extern Room328Vec4 D_8018EFFC;
extern s16 D_800942EC;
extern char D_801921A0;

char *func_800C2B50(void);
void func_80078CC4(Room328Matrix *matrix, Room328Vec4 *scale);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, Room328Matrix *matrix, s32 arg2);

void func_8018F9A4(void) {
    Room328Matrix matrix;
    Room328Vec4 scale;
    char *owner;

    owner = func_800C2B50();
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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
    scale = D_8018EFFC;
    func_80078CC4(&matrix, &scale);

    matrix.t[0] = *(s32 *)(owner + 0x18);
    matrix.t[1] = D_800942EC;
    matrix.t[2] = *(s32 *)(owner + 0x20);
    func_800C42A4(&D_801921A0, &matrix, 1);
}
