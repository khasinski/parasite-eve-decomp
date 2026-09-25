#include "pe1/room_orbiting_pair.h"

int func_80190230(int mode, void *unused, s32 *state) {
    RoomOrbitPairTemplate8 template = D_8018F03C;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomOrbitPairParticle *child;
    if (mode == 1)
        goto update;
    if (mode < 2) {
        if (mode == 0)
            goto init;
        goto done;
    }
    if (mode == 2)
        goto configure;
    goto done;
init:
    if (D_800E2368->active) {
        RoomOrbitPairNode **slot = (RoomOrbitPairNode **)D_800F32D0->pool;
        if (slot && *slot) {
            u8 *flag = (*slot)->state;
            if (*flag == 1) *flag = 2;
        }
    }
    pool = D_800F33E0->pool;
    return func_800CE560(pool, 20, 46, func_8018F058);
update:
    pool = D_800F32D0->pool;
    func_800CE8F0(pool, 7, &template, position);
    pool = D_800F32D0->pool;
    func_800CE9D4(pool, 0, target);
    D_80192BFC = -(u16)target[1] + 0x400;
    if (D_800E27EC == 1) {
        pool = D_800F33E0->pool;
        child = func_800CE610(pool);
        if (child) {
            int radius = *state;
            child->x = position[0];
            child->y = position[1];
            child->z = position[2];
            child->vx = rcos(D_80192BFC) * radius / 4096;
            child->vz = rsin(D_80192BFC) * radius / 4096;
            child->vy = 24;
            child->state = 0;
            child->timer = 0;
        }
        pool = D_800F33E0->pool;
        child = func_800CE610(pool);
        if (child) {
            child->x = position[0];
            child->y = position[1];
            child->z = position[2];
            child->state = 5;
            child->timer = 0;
        }
    }
    if (D_800E27EC < 2) goto done;
    return 2;
configure:
    {
        int palette = D_800E2850[D_800E11EA];
        PE1_COMPILER_MEMORY_BARRIER();
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 8;
        D_800F3370 = palette;
    }
done:
    return 0;
}
