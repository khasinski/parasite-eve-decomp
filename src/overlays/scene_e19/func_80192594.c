#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/room_m023_beacon.h"
#include "pe1/scene_grab_head.h"

extern char D_8019B31C[];
extern char D_8019B2FC[];
extern s32 D_8019B310;
extern s32 D_8019B314;
extern s32 D_8019B318;

s32 func_80192594(char *obj) {
    *(void **)(obj + 0xC) = SceneEffect_GrabAwaitStance;
    obj[0x1A] = 0;
    obj[0x44] = 0;
    obj[0x3] = 0xFF;
    RotMatrixYXZ(D_8019B31C, (void *)0x1F800028);
    MulMatrix0(D_8019B2FC, (void *)0x1F800028, obj + 0x1C);
    *(s32 *)(obj + 0x30) = D_8019B310;
    *(s32 *)(obj + 0x34) = D_8019B314;
    *(s32 *)(obj + 0x38) = D_8019B318;
    return 0;
}
