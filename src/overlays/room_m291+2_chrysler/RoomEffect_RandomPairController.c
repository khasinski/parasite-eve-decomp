#include "pe1/room_orbiting_pair.h"

int func_801908B0(int mode, void *unused, void *state) {
    RoomOrbitPairTemplate8 template = D_8018F03C;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomOrbitPairParticle *child;
    int angle, radius;
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
    {
        int handle = func_800D3FD8();
        func_800D3F64(0x58B, handle);
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 20, 16, func_8019050C);
    }
update:
    pool = D_800F32D0->pool;
    func_800CE8F0(pool, 7, &template, position);
    pool = D_800F32D0->pool;
    func_800CE9D4(pool, 0, target);
    angle = -target[1] + 0x400;
    if (D_800E27EC < 47) {
        if (D_800E27EC & 1) {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = position[0];
                child->y = position[1];
                child->z = position[2];
                radius = (func_80071A54() & 15) + 8;
                angle = angle + (func_80071A54() & 0x1FF) - 0x100;
                child->vx = rcos(angle) * radius / 4096;
                child->vz = rsin(angle) * radius / 4096;
                child->vy = -(func_80071A54() & 7);
                child->state = 0;
                child->timer = 0;
            }
        } else {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = position[0];
                child->y = position[1];
                child->z = position[2];
                radius = (func_80071A54() & 7) + 4;
                angle = angle + (func_80071A54() & 0x1FF) - 0x100;
                child->vx = rcos(angle) * radius / 4096;
                child->vz = rsin(angle) * radius / 4096;
                child->vy = -(func_80071A54() & 15);
                child->state = 1;
                child->timer = 0;
            }
        }
    }
    if (D_800E27EC < 2) goto done;
    return 2;
configure:
    {
        int idx = D_800E11EA;
        int palette;
        D_800F3368 = 32;
        D_800F336A = 2;
        D_800F3376 = 32;
        D_800F3378 = 32;
        palette = D_800E2850[idx];
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
