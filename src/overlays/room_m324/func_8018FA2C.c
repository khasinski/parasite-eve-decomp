typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef int s32;

typedef struct Room137Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Room137Matrix;

typedef struct Room137Seed {
    char bytes[8];
} Room137Seed;

typedef struct Room137Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Room137Vec4;

typedef struct Room137FxParams {
    s16 x;
    s16 y;
    s16 z;
    char pad6[0xA];
    s16 scale;
    u16 depth;
    char pad14;
    u8 alpha;
} Room137FxParams;

extern Room137Seed D_8018F014;
extern u8 D_80190F64;
extern u16 D_80190F6A;

char *func_800C2B50(void);
void func_80071A44(void *arg0, s32 arg1, s32 arg2);
void func_800794C4(Room137Seed *seed, Room137Matrix *matrix);
void func_80078CC4(Room137Matrix *matrix, Room137Vec4 *scale);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, Room137Matrix *matrix, s32 arg2);

void func_8018FA2C(void *arg0, void *arg1, Room137FxParams *fx) {
    Room137Matrix matrix;
    Room137Seed seed;
    Room137Vec4 scratch_scale;
    Room137Vec4 scale;
    char *owner;
    register u16 *depth_slot asm("$4");

    owner = func_800C2B50();
    seed = D_8018F014;
    func_800C2EAC(owner[0x10]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(1);
    func_800794C4(&seed, &matrix);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x1000;
    scratch_scale = scale;
    func_80078CC4(&matrix, &scratch_scale);

    depth_slot = &D_80190F6A;
    *depth_slot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    D_80190F64 = fx->alpha << 1;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 0);
}
