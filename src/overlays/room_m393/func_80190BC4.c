typedef struct RoomM393FxSeed {
    char bytes[8];
} RoomM393FxSeed;

typedef struct RoomM393FxMatrix {
    short m[3][3];
    short pad;
    int t[3];
} RoomM393FxMatrix;

typedef struct RoomM393FxVec4 {
    int x;
    int y;
    int z;
    int w;
} RoomM393FxVec4;

typedef struct RoomM393FxParams {
    short x;
    short y;
    short z;
    char pad6[0xA];
    short scale;
    unsigned short depth;
    char pad14;
    unsigned char alpha;
} RoomM393FxParams;

extern RoomM393FxSeed D_8018F01C;
extern unsigned char D_8019484C;
extern unsigned short D_80194852;

char *func_800C2B50(void);
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800794C4(RoomM393FxSeed *seed, RoomM393FxMatrix *matrix);
void func_80071A44(void *arg0, int arg1, int arg2);
void func_80078CC4(RoomM393FxMatrix *matrix, RoomM393FxVec4 *scale);
void func_800C42A4(void *arg0, RoomM393FxMatrix *matrix, int arg2);

void func_80190BC4(void *arg0, void *arg1, RoomM393FxParams *fx) {
    RoomM393FxMatrix matrix;
    RoomM393FxSeed seed;
    RoomM393FxVec4 scratch_scale;
    RoomM393FxVec4 scale;
    char *owner;
    register unsigned short *depth_slot asm("$4");

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

    depth_slot = &D_80194852;
    *depth_slot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    D_8019484C = fx->alpha << 1;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 0);
}
