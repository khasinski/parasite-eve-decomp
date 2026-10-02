#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_LIFTED_SPARK_CTRL_FUNC
#error "ROOMEFFECT_LIFTED_SPARK_CTRL_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_LIFTED_SPARK_CTRL_CALLBACK
#error "ROOMEFFECT_LIFTED_SPARK_CTRL_CALLBACK must name the particle callback"
#endif

extern void ROOMEFFECT_LIFTED_SPARK_CTRL_CALLBACK(void);

/* Controller that sprays lifted sparks from the actor on odd frames of the
 * first minute, bursts sixteen more on frame 48, plays the sound on frame
 * two, and configures the sprite palette. */
int ROOMEFFECT_LIFTED_SPARK_CTRL_FUNC(int mode) {
    GteShortVector target;
    RoomDampedSpark *child;
    char *pool;
    int handle;
    int i;

    switch (mode) {
    case 0:
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 20, 32, ROOMEFFECT_LIFTED_SPARK_CTRL_CALLBACK);
    case 1:
        pool = D_800F32D0->pool;
        func_800CE870(pool, 0, &target);
        if ((D_800E27EC & 1) && D_800E27EC < 60) {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = target.x;
                child->y = target.y;
                child->z = target.z;
                child->vx = func_80071A54() % 80 - 40;
                child->vy = -(func_80071A54() % 60);
                child->vz = func_80071A54() % 80 - 40;
                child->state = 1;
                child->timer = 0;
            }
        }
        if (D_800E27EC == 48) {
            for (i = 0; i < 16; i++) {
                pool = D_800F33E0->pool;
                child = func_800CE610(pool);
                if (child) {
                    child->x = target.x;
                    child->y = target.y;
                    child->z = target.z;
                    child->vx = func_80071A54() % 192 - 96;
                    child->vy = -(func_80071A54() % 170);
                    child->vz = func_80071A54() % 192 - 96;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 2) {
            handle = func_800D3FD8();
            func_800D3F64(0x5B4, handle);
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2: {
        int index = D_800E11EA;
        int tpage = D_800E2850[index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = tpage;
        break;
    }
    }
    return 0;
}

#undef ROOMEFFECT_LIFTED_SPARK_CTRL_FUNC
#undef ROOMEFFECT_LIFTED_SPARK_CTRL_CALLBACK
