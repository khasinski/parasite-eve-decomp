typedef unsigned short u16;
typedef signed short s16;
typedef int s32;

typedef struct Room034Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Room034Matrix;

typedef struct Room034Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Room034Vec4;

typedef struct Room034FxParams {
    char pad0[4];
    u16 depth;
    char pad6[2];
    s16 x;
    s16 y;
    s16 z;
} Room034FxParams;

extern Room034Vec4 D_8018EFFC;
extern u16 D_8019006A;

void func_80078CC4(Room034Matrix *matrix, Room034Vec4 *scale);
void func_800C2EAC(s32 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, Room034Matrix *matrix, s32 arg2);

void func_8018FDE4(void *arg0, void *arg1, Room034FxParams *fx) {
    Room034Matrix matrix;
    Room034Vec4 scale;
    u16 *depth_slot;

    func_800C2EAC(0);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
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

    depth_slot = &D_8019006A;
    *depth_slot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    scale = D_8018EFFC;
    func_80078CC4(&matrix, &scale);
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 1);
}
