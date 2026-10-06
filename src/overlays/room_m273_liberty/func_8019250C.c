#include "pe1/gte_types.h"
#include "pe1/room_m023_beacon.h"
#include "pe1/scene_grab_head.h"
extern char D_8019A8B8;
extern int D_8019A8CC;
extern int D_8019A8D0;
extern int D_8019A8D4;
extern char D_8019A8D8;


int func_8019250C(char *obj) {
    int ret;

    *(void **)(obj + 0x0C) = SceneEffect_GrabAwaitStance;
    obj[0x1A] = 0;
    obj[0x4A] = 0;
    *(short *)(obj + 0x48) = 0;
    obj[0x4B] = 0;
    obj[3] = 0xFF;
    RotMatrixYXZ(&D_8019A8D8, (void *)0x1F800028);

    ret = MulMatrix0(&D_8019A8B8, (void *)0x1F800028, obj + 0x1C);
    *(int *)(obj + 0x30) = D_8019A8CC;
    *(int *)(obj + 0x34) = D_8019A8D0;
    *(int *)(obj + 0x38) = D_8019A8D4;
    /* Keep the call result live so GCC allocates the final data load to v1. */
    return ret - ret;
}
