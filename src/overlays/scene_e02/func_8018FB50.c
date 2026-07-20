typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct {
    /* 0x00 */ s16 m[3][3];
    /* 0x12 */ s16 pad;
    /* 0x14 */ s32 t[3];
} Matrix;

typedef struct {
    /* 0x0 */ s32 x, y, z, w;
} Vec4;

char *func_800C2B50(void);
void func_800C2EAC(s32 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_80078CC4(Matrix *m, Vec4 *v);
s32 func_800C42A4(void *arg0, Matrix *m, s32 arg2);

extern Vec4 D_8018EFFC;
extern char D_801956F8[];
extern s16 D_800942EC;

void func_8018FB50(void) {
    char *view;
    Matrix m;
    Vec4 v;

    view = func_800C2B50();
    func_800C2EAC((u8)view[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    m.m[2][2] = 0x1000;
    m.m[1][1] = 0x1000;
    m.m[0][0] = 0x1000;
    m.t[2] = 0;
    m.t[1] = 0;
    m.t[0] = 0;
    m.m[2][1] = 0;
    m.m[2][0] = 0;
    m.m[1][2] = 0;
    m.m[1][0] = 0;
    m.m[0][2] = 0;
    m.m[0][1] = 0;
    v = D_8018EFFC;
    func_80078CC4(&m, &v);
    m.t[0] = *(s32 *)(view + 0x18);
    m.t[1] = D_800942EC;
    m.t[2] = *(s32 *)(view + 0x20);
    func_800C42A4(D_801956F8, &m, 1);
}
