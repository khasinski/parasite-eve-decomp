#include "pe1/room_m404.h"

int func_80193368(int mode, void *unused, char *state) {
    RoomTemplate8 template = D_8018F208;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomM404Particle *child;
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
        func_800D3F64(0x5b9, handle);
        if (D_800E2368->active) {
            RoomM404Node **slot = (RoomM404Node **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 20, 55, func_80192540);
    }
update:
    {
        pool = D_800F32D0->pool;
        func_800CE8F0(pool, 3, &template, position);
        pool = D_800F32D0->pool;
        func_800CE9D4(pool, 0, target);
        if (D_800E27EC == 1) {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = position[0];
                child->y = position[1];
                child->z = position[2];
                func_800CFB7C(target, *(s16 *)state, &child->vx);
                child->state = 0;
                child->timer = 0;
            }
        }
        if (D_800E27EC < 2) goto done;
        return 2;
    }
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
