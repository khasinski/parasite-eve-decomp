typedef struct RoomLibEffectMatrix {
    short m[3][3];
    short pad12;
    int t[3];
} RoomLibEffectMatrix;

typedef struct RoomLibEffectView {
    char pad00[0x1A0];
    RoomLibEffectMatrix matrix;
} RoomLibEffectView;

typedef struct RoomLibEffectRoot {
    char pad00[0x238];
    RoomLibEffectView *view;
} RoomLibEffectRoot;

typedef struct RoomLibEffectRootTable {
    RoomLibEffectRoot *root;
    char pad04[0x20];
    unsigned char mode;
} RoomLibEffectRootTable;

typedef struct RoomLibScaledEffect {
    short x;
    short y;
    short z;
    char pad06[0xA];
    unsigned short depth;
    unsigned short scaleTimer;
} RoomLibScaledEffect;

typedef struct RoomLibMatrixSeed {
    char bytes[8];
} RoomLibMatrixSeed;

typedef struct RoomLibVectorSeed {
    int words[4];
} RoomLibVectorSeed;

extern RoomLibMatrixSeed D_8018F01C;
extern RoomLibVectorSeed D_8018F024;
extern short D_80192C30;
extern unsigned short D_80192C32;
extern void func_80078C34(void *, RoomLibMatrixSeed *, short *);
extern void func_800C2EAC(unsigned char);
extern void func_800C2FF0(int, int);
extern void func_800C3098(int);
extern void func_800C3238(int);
extern void func_80071A44(void *, int, int);

void func_80192794(void *unused0, void *unused1, RoomLibScaledEffect *effect) {
    RoomLibEffectRootTable *roots;
    RoomLibEffectView *view;
    RoomLibEffectMatrix matrix;
    RoomLibMatrixSeed seed;
    short offset[4];
    RoomLibVectorSeed vectorSeed;
    RoomLibVectorSeed scaleSeed;
    unsigned short *depth;
    int scale;

    roots = (RoomLibEffectRootTable *)func_800C2B50();
    view = roots->root->view;
    seed = D_8018F01C;
    matrix = view->matrix;
    vectorSeed = D_8018F024;
    func_80078CC4((RoomSpriteMatrix *)&matrix, (RoomFxVec4 *)&vectorSeed);
    func_80078C34(&view->matrix, &seed, offset);
    matrix.t[0] += offset[0];
    matrix.t[1] += offset[1];
    matrix.t[2] += offset[2];

    func_800C2EAC(roots->mode);
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

    scale = (short)effect->scaleTimer >> 2;
    func_80071A44(&scaleSeed, 0, 0x10);
    scaleSeed.words[0] = scale * 3;
    scaleSeed.words[1] = scale * 3;
    scaleSeed.words[2] = scale * 3;
    func_80078CC4((RoomSpriteMatrix *)&matrix, (RoomFxVec4 *)&scaleSeed);

    depth = &D_80192C32;
    matrix.t[0] = effect->x;
    matrix.t[1] = effect->y;
    matrix.t[2] = effect->z;
    D_80192C30 = -0x65;
    *depth = effect->depth;
    func_800C42A4((char *)depth - 0xA, (RoomSpriteMatrix *)&matrix, 1);
}
