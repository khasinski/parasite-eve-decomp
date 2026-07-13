typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;

typedef struct {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
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
    s16 x;
    s16 y;
    s16 z;
    char pad6[0xA];
    u16 depth;
    u16 scale_timer;
} Overlay024Effect;

typedef struct {
    char bytes[8];
} Overlay024MatrixSeed;

typedef struct {
    s32 words[4];
} Overlay024VectorSeed;

extern Overlay024MatrixSeed D_8018F01C;
extern Overlay024VectorSeed D_8018F024;
extern s16 D_80192C30;
extern u16 D_80192C32;

Overlay024Root **func_800C2B50(void);
void func_80078CC4(Overlay024Matrix *matrix, Overlay024VectorSeed *seed);
void func_80078C34(void *transform, Overlay024MatrixSeed *seed, short *out);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_80071A44(void *arg0, s32 arg1, s32 arg2);
void func_800C42A4(void *arg0, Overlay024Matrix *matrix, s32 arg2);

void func_80192794(void *arg0, void *arg1, Overlay024Effect *effect) {
    Overlay024Root **root;
    Overlay024View *view;
    Overlay024Matrix matrix;
    Overlay024MatrixSeed seed;
    short out[4];
    Overlay024VectorSeed vector_seed;
    Overlay024VectorSeed scale_seed;
    u16 *depth_slot;
    s32 scale;

    root = func_800C2B50();
    view = (*root)->view;

    seed = D_8018F01C;
    matrix = view->matrix;
    vector_seed = D_8018F024;

    func_80078CC4(&matrix, &vector_seed);
    func_80078C34(&view->matrix, &seed, out);

    matrix.t[0] += out[0];
    matrix.t[1] += out[1];
    matrix.t[2] += out[2];

    func_800C2EAC(*(u8 *)((char *)root + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C3238(3);

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

    scale = (s16)effect->scale_timer >> 2;
    func_80071A44(&scale_seed, 0, 0x10);
    scale_seed.words[0] = scale * 3;
    scale_seed.words[1] = scale * 3;
    scale_seed.words[2] = scale * 3;
    func_80078CC4(&matrix, &scale_seed);

    depth_slot = &D_80192C32;
    matrix.t[0] = effect->x;
    matrix.t[1] = effect->y;
    matrix.t[2] = effect->z;
    D_80192C30 = -0x65;
    *depth_slot = effect->depth;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 1);
}
