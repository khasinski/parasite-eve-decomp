#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_JITTER_SPAWNER_FUNC
#error "ROOMEFFECT_JITTER_SPAWNER_FUNC must name the spawner"
#endif
#ifndef ROOMEFFECT_JITTER_SPAWNER_CALLBACK
#error "ROOMEFFECT_JITTER_SPAWNER_CALLBACK must name the jitter sized spark"
#endif

extern int ROOMEFFECT_JITTER_SPAWNER_CALLBACK(int mode, RoomJitterSpark *spark);

/* Spawner that, while the primary scene object is past frame 10, scatters
 * up to three jitter sized sparks a frame up the object's anchor. */
int ROOMEFFECT_JITTER_SPAWNER_FUNC(int mode) {
    GteShortVector position;
    RoomJitterSpark *child;
    int i;

    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool, 20, 10, ROOMEFFECT_JITTER_SPAWNER_CALLBACK);
    case 1:
        if (D_800F32D0 != D_800942E4) return 1;
        if (((RoomSparkOwner *)D_800F32D0->pool)->state->frame < 11) return 1;
        func_800CE870(D_800F32D0->pool, 0, (s16 *)&position);
        for (i = 0; i < 3; i++) {
            if (func_80071A54() & 1) {
                child = (RoomJitterSpark *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    position.y += 110;
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->x += func_80071A54() % 400 - 200;
                    child->y += func_80071A54() % 800 - 400;
                    child->z += func_80071A54() % 400 - 200;
                    child->size = (func_80071A54() & 0x1FF) + 682;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 24 - 12;
                    child->vy = -(func_80071A54() % 24);
                    child->vz = func_80071A54() % 24 - 12;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        break;
    case 2: {
        int tpage;
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        D_800F3368.tpage = tpage;
        break;
    }
    }
    return 0;
}

#undef ROOMEFFECT_JITTER_SPAWNER_FUNC
#undef ROOMEFFECT_JITTER_SPAWNER_CALLBACK
