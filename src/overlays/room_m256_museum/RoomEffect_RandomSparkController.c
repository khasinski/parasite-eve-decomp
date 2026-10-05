#include "pe1/room_orbiting_pair.h"

int func_801934AC(int mode, u16 *counter) {
    RoomOrbitPairTemplate8 template = D_8018F208;
    s16 position[4];
    s16 target[4];
    s16 angles[4];
    char *pool;
    RoomOrbitSpark *child;
    int distance;
    int spread;
    switch (mode) {
    case 0:
    {
        RoomOrbitPairEventState *event = D_800E2368;
        *counter = 0;
        if (event->active) {
            RoomOrbitPairNode **slot = (RoomOrbitPairNode **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
    }
    pool = D_800F33E0->pool;
    return func_800CE560(pool, 20, 50, func_80192844);
    case 1:
    pool = D_800F32D0->pool;
    func_800CE8F0(pool, 4, &template, position);
    pool = D_800F32D0->pool;
    func_800CE9D4(pool, 4, target);
    target[0] -= 0xDC;
    if (D_800E27EC < 110 && (D_800E27EC & 1)) {
        pool = D_800F33E0->pool;
        child = func_800CE610(pool);
        if (child) {
            distance = (func_80071A54() & 7) + 0x37;
            child->x = position[0];
            child->y = position[1];
            child->z = position[2];
            angles[0] = target[0];
            angles[1] = target[1];
            angles[2] = target[2];
            spread = func_80071A54() & 0x1F;
            angles[1] += spread - 0x10;
            func_800CFB7C(angles, distance, &child->vx);
            child->parity = *counter & 1;
            child->state = 0;
            child->timer = 0;
        }
        (*counter)++;
    }
    if (D_800E27EC < 2) break;
    return 2;
    case 2:
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
        break;
    }
    return 0;
}
