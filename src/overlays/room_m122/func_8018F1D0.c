#include "common.h"
typedef struct {
    s32 w[8];
} FxViewSnapshot;

extern void func_800C2B40(void *arg0);
extern s32 func_8006DC18(s32 arg0);
extern s32 func_8006E498(s32 arg0, s32 arg1);

extern s32 D_800B0E64;
extern u8 D_801910A8;
extern u8 D_801910A9;
extern u8 D_801910AA;
extern u8 D_801910AC;
extern u8 D_801910AD;
extern u8 D_801910AE;
extern s16 D_801910B0;
extern s16 D_801910B2;
extern s32 D_801910B4;

s32 func_8018F1D0(char *obj, s32 arg1, char *state) {
    char *owner;

    func_800C2B40(state);
    owner = *(char **)(obj + 0x8);
    *(char **)(state + 0x0) = owner;
    *(FxViewSnapshot *)(state + 0x4) = **(FxViewSnapshot **)(owner + 0x238);
    *(s16 *)(state + 0x2A) = 0;
    *(s16 *)(state + 0x2C) = 0;
    *(s16 *)(state + 0x28) = 0x1E;
    *(s32 *)(state + 0x24) = func_8006DC18(0x23);
    D_801910B0 = -0x12C;
    D_801910B2 = 0x80;
    D_801910AC = 0;
    D_801910AD = 0;
    D_801910A8 = 0x80;
    D_801910A9 = 0x80;
    D_801910AA = 0x80;
    D_801910AE = 0;
    return D_801910B4 = func_8006E498(D_800B0E64, 0xCB8704);
}
