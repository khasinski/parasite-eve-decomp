typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

typedef struct {
    s32 m00;
    s32 m04;
    s32 m08;
    s32 m0c;
    s32 m10;
    s32 tx;
    s32 ty;
    s32 tz;
} Overlay024Matrix;

typedef struct {
    char pad0[0x1A0];
    Overlay024Matrix matrix;
} Overlay024View;

typedef struct {
    char pad0[0x238];
    Overlay024View *view;
} Overlay024Root;

typedef struct {
    char bytes[8];
} Overlay024MatrixSeed;

typedef struct {
    s32 words[4];
} Overlay024VectorSeed;

extern Overlay024MatrixSeed D_8018F01C;
extern Overlay024VectorSeed D_8018F024;
extern void *D_80192C34;

Overlay024Root **func_800C2B50(void);
void func_80078CC4(Overlay024Matrix *matrix, Overlay024VectorSeed *seed);
void func_80078C34(void *transform, Overlay024MatrixSeed *seed, short *out);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C6D5C(void *arg0, s32 arg1, s32 arg2);
void func_800C6EE8(s32 arg0);
s32 func_80077A64(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80077AA4(s32 arg0, s32 arg1);
void func_800C6EC0(s32 arg0, s32 arg1);
void func_800C6ED8(s32 arg0);
void func_800C6EF8(void *arg0);
void func_800C6FA0(void *arg0, s32 arg1);
void func_800C71E4(void *arg0, Overlay024Matrix *matrix);
void func_800C6F4C(void *arg0);

void func_801034B4(void) {
    Overlay024Root **root;
    Overlay024View *view;
    Overlay024Matrix matrix;
    Overlay024MatrixSeed seed;
    short out[4];
    Overlay024VectorSeed vector_seed;
    s32 handle_a;
    s32 handle_b;

    root = func_800C2B50();
    view = (*root)->view;

    seed = D_8018F01C;
    matrix = view->matrix;
    vector_seed = D_8018F024;

    func_80078CC4(&matrix, &vector_seed);
    func_80078C34(&view->matrix, &seed, out);

    matrix.tx += out[0];
    matrix.ty += out[1];
    matrix.tz += out[2];

    func_800C2EAC(*(u8 *)((char *)root + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C6D5C(D_80192C34, 0, 0);
    func_800C6EE8(-10);
    handle_a = func_80077A64(0, 1, 0x340, 0x100);
    handle_a = (u16)handle_a;
    handle_b = func_80077AA4(0x10, 0x1D7);
    func_800C6EC0(handle_a, (u16)handle_b);
    func_800C6ED8(0);
    func_800C6EF8(D_80192C34);
    func_800C6FA0(D_80192C34, 0x80);
    func_800C71E4(D_80192C34, &matrix);
    func_800C6F4C(D_80192C34);
}
