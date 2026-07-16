typedef struct RoomM146FxSeed {
    char bytes[8];
} RoomM146FxSeed;

typedef struct RoomM146FxMatrix {
    short m[3][3];
    short pad;
    int t[3];
} RoomM146FxMatrix;

typedef struct RoomM146FxVec4 {
    int x;
    int y;
    int z;
    int w;
} RoomM146FxVec4;

typedef struct RoomM146FxParams {
    short x;
    short y;
    short z;
    char pad6[0xA];
    short scale;
    unsigned short depth;
    char pad14;
    unsigned char alpha;
} RoomM146FxParams;

extern RoomM146FxSeed D_8018F064;
extern unsigned char D_8019269C;
extern unsigned short D_801926A2;

char *func_800C2B50(void);
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800794C4(RoomM146FxSeed *seed, RoomM146FxMatrix *matrix);
void func_80071A44(void *arg0, int arg1, int arg2);
void func_80078CC4(RoomM146FxMatrix *matrix, RoomM146FxVec4 *scale);
void func_800C42A4(void *arg0, RoomM146FxMatrix *matrix, int arg2);

void func_80190EFC(void *arg0, void *arg1, RoomM146FxParams *fx) {
    RoomM146FxMatrix matrix;
    RoomM146FxSeed seed;
    RoomM146FxVec4 scratch_scale;
    RoomM146FxVec4 scale;
    char *owner;
    unsigned short *depth_slot;

    owner = func_800C2B50();
    seed = D_8018F064;
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

    depth_slot = &D_801926A2;
    *depth_slot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    D_8019269C = fx->alpha << 1;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 0);
}
