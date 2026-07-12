typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;

typedef struct SceneE02Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} SceneE02Matrix;

typedef struct SceneE02Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} SceneE02Vec4;

typedef struct SceneE02Pos {
    char pad0[0x18];
    s32 x;
    s32 y;
    s32 z;
    u8 mode;
} SceneE02Pos;

extern volatile s32 D_8018EFFC[4];
extern char D_801956F8;
extern s16 D_800942EC;

SceneE02Pos *func_800C2B50(void);
void func_80078CC4(SceneE02Matrix *matrix, SceneE02Vec4 *scale);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, SceneE02Matrix *matrix, s32 arg2);

void func_8018FB50(void *arg0, void *arg1, char *arg2) {
    SceneE02Matrix matrix;
    SceneE02Vec4 scale;
    register SceneE02Pos *pos asm("$16");
    register volatile s32 *src asm("$5");
    register s32 sx asm("$2");
    register s32 sy asm("$3");
    s32 sz;

    pos = func_800C2B50();
    func_800C2EAC(pos->mode);
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

    __asm__ volatile("");
    src = D_8018EFFC;
    __asm__ volatile("" : "=r"(src) : "0"(src));
    sx = src[0];
    sy = src[1];
    sz = src[2];
    scale.x = sx;
    scale.y = sy;
    scale.z = sz;
    sx = src[3];
    __asm__ volatile("" : "=r"(sx) : "0"(sx));
    scale.w = sx;
    __asm__ volatile("");
    func_80078CC4(&matrix, &scale);

    matrix.t[0] = pos->x;
    matrix.t[1] = D_800942EC;
    matrix.t[2] = pos->z;
    func_800C42A4(&D_801956F8, &matrix, 1);
}
