typedef short s16;
typedef unsigned char u8;
typedef int s32;

typedef struct RoomWord4 {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
} RoomWord4;

typedef struct RoomPos {
    RoomWord4 lo;
    RoomWord4 hi;
} RoomPos;

typedef struct RoomViewData {
    RoomPos pos;
    char pad20[0x114];
    s32 x134;
    char pad138[4];
    s32 z13C;
    char pad140[0x74];
    s32 x1B4;
    char pad1B8[4];
    s32 z1BC;
} RoomViewData;

typedef struct RoomDrawObject {
    void *asset;
    s16 h4;
    s16 h6;
    s16 h8;
    s16 hA;
    s16 hC;
    char padE[2];
    u8 b10;
    u8 b11;
    u8 b12;
    char pad13;
    RoomWord4 word14;
    RoomWord4 word24;
} RoomDrawObject;

extern char *func_800C2B50(void);
extern void func_800C5538(RoomDrawObject *obj);
extern void *func_800C66C8(void *obj, int asset, s32 *pos);
extern s16 D_800942EC;
extern char D_801940C8[];
extern char D_80194370[];

#define ROOT_OWNER(root) (*(char **)(root))
#define ROOT_VIEW(root) (*(RoomViewData **)(ROOT_OWNER(root) + 0x238))

void func_80190884(void *arg0, void *arg1, char *sys) {
    register void *obj asm("s7") = arg0;
    register char *sysp asm("s0") = sys;
    register int c10 asm("s6");
    register int c190 asm("s5");
    register int c12c asm("s4");
    register int c20 asm("s3");
    register int c2 asm("s2");
    register int c1 asm("s1");
    register RoomViewData *view asm("v1");
    char *root;
    RoomPos pos0;
    RoomPos pos1;
    RoomDrawObject *draw0;
    RoomDrawObject *draw1;

    root = func_800C2B50();

    view = ROOT_VIEW(root);
    pos1 = view->pos;

    view = ROOT_VIEW(root);
    pos0 = view->pos;

    view = ROOT_VIEW(root);
    pos1.hi.w1 = view->x134;
    pos1.hi.w2 = D_800942EC;
    view = ROOT_VIEW(root);
    pos1.hi.w3 = view->z13C;
    view = ROOT_VIEW(root);
    pos0.hi.w1 = view->x1B4;
    pos0.hi.w2 = D_800942EC;
    c10 = 10;
    c190 = 0x190;
    c12c = 0x12C;
    c2 = 2;
    c20 = 0x20;
    c1 = 1;
    pos0.hi.w3 = ROOT_VIEW(root)->z1BC;

    draw0 = (RoomDrawObject *)(sysp + 0xAC);
    draw0->h4 = c10;
    draw0->asset = D_80194370;
    draw0->h6 = 0;
    draw0->h8 = c190;
    draw0->hA = c12c;
    draw0->hC = c2;
    draw0->word14 = pos0.lo;
    draw0->word24 = pos0.hi;
    draw0->b10 = c20;
    draw0->b11 = c1;
    draw0->b12 = c1;
    func_800C5538(draw0);

    draw1 = (RoomDrawObject *)(sysp + 0x78);
    draw1->h4 = c10;
    draw1->asset = D_801940C8;
    draw1->h6 = 0;
    draw1->h8 = c190;
    draw1->hA = c12c;
    draw1->hC = c2;
    draw1->word14 = pos1.lo;
    draw1->word24 = pos1.hi;
    draw1->b10 = c20;
    draw1->b11 = c1;
    draw1->b12 = c1;
    func_800C5538(draw1);

    func_800C66C8(obj, 0x576, (s32 *)&pos1);
}
