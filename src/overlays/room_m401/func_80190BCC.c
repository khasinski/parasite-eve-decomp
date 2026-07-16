typedef struct RoomM401FxSeed {
    char bytes[8];
} RoomM401FxSeed;

typedef struct RoomM401FxMatrix {
    short m[3][3];
    short pad;
    int t[3];
} RoomM401FxMatrix;

typedef struct RoomM401FxVec4 {
    int x;
    int y;
    int z;
    int w;
} RoomM401FxVec4;

typedef struct RoomM401FxParams {
    short x;
    short y;
    short z;
    char pad6[0xA];
    short scale;
    unsigned short depth;
    char pad14;
    unsigned char alpha;
} RoomM401FxParams;

extern RoomM401FxSeed D_8018F01C;
extern unsigned char D_801955B4;
extern unsigned short D_801955BA;

char *func_800C2B50(void);
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800794C4(RoomM401FxSeed *seed, RoomM401FxMatrix *matrix);
void func_80071A44(void *arg0, int arg1, int arg2);
void func_80078CC4(RoomM401FxMatrix *matrix, RoomM401FxVec4 *scale);
void func_800C42A4(void *arg0, RoomM401FxMatrix *matrix, int arg2);

void func_80190BCC(void *arg0, void *arg1, RoomM401FxParams *fx) {
    RoomM401FxMatrix matrix;
    RoomM401FxSeed seed;
    RoomM401FxVec4 scratch_scale;
    RoomM401FxVec4 scale;
    char *owner;
    unsigned short *depth_slot;

    owner = func_800C2B50();
    seed = D_8018F01C;
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

    depth_slot = &D_801955BA;
    *depth_slot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    D_801955B4 = fx->alpha << 1;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 0);
}
