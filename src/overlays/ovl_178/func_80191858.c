typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct Overlay178Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Overlay178Matrix;

typedef struct Overlay178Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Overlay178Vec4;

extern s32 D_8019956C;
extern s32 D_8019957C;
extern char D_801996A0;

void *func_800C2B50(void);
void func_80071A44(void *arg0, s32 arg1, s32 arg2);
void func_80078CC4(Overlay178Matrix *matrix, Overlay178Vec4 *scale);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, Overlay178Matrix *matrix, s32 arg2);

void func_80191858(void *arg0, void *arg1, char *arg2) {
    Overlay178Matrix matrix;
    Overlay178Vec4 scratch_scale;
    Overlay178Vec4 draw_scale;
    char *owner;

    owner = func_800C2B50();
    func_800C2EAC(*(u8 *)(owner + 0x24));
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

    func_80071A44(&draw_scale, 0, 0x10);
    draw_scale.x = *(s16 *)(arg2 + 0x8);
    draw_scale.y = *(s16 *)(arg2 + 0x8);
    draw_scale.z = 0x1000;
    scratch_scale = draw_scale;
    func_80078CC4(&matrix, &scratch_scale);

    matrix.t[0] = *(s16 *)(arg2 + 0x0) + D_8019956C;
    matrix.t[1] = *(s16 *)(arg2 + 0x2);
    matrix.t[2] = *(s16 *)(arg2 + 0x4) + D_8019957C;
    func_800C42A4(&D_801996A0, &matrix, 1);
}
