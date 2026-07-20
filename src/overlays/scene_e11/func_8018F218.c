typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct {
    s32 w[8];
} FxViewSnapshot;

void func_800C2B40(void *arg0);
s32 func_8006DC18(s32 arg0);
s32 func_8006E498(s32 arg0, s32 arg1);

extern s32 D_800B0E64;
extern u8 D_80191BD8;
extern u8 D_80191BD9;
extern u8 D_80191BDA;
extern u8 D_80191BDC;
extern u8 D_80191BDD;
extern u8 D_80191BDE;
extern s16 D_80191BE0;
extern s16 D_80191BE2;
extern s32 D_80191BE4;

s32 func_8018F218(char *obj, s32 arg1, char *state) {
    char *owner;

    func_800C2B40(state);
    owner = *(char **)(obj + 0x8);
    *(char **)(state + 0x0) = owner;
    *(FxViewSnapshot *)(state + 0x4) = **(FxViewSnapshot **)(owner + 0x238);
    *(s16 *)(state + 0x2A) = 0;
    *(s16 *)(state + 0x2C) = 0;
    *(s16 *)(state + 0x28) = 0x1E;
    *(s32 *)(state + 0x24) = func_8006DC18(0x23);
    D_80191BE0 = -0x12C;
    D_80191BE2 = 0x80;
    D_80191BDC = 0;
    D_80191BDD = 0;
    D_80191BD8 = 0x80;
    D_80191BD9 = 0x80;
    D_80191BDA = 0x80;
    D_80191BDE = 0;
    return D_80191BE4 = func_8006E498(D_800B0E64, 0xCB8704);
}
