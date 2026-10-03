#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_COMET_SPARK_CTRL_FUNC
#error "ROOMEFFECT_COMET_SPARK_CTRL_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_COMET_SPARK_CTRL_CALLBACK
#error "ROOMEFFECT_COMET_SPARK_CTRL_CALLBACK must name the particle callback"
#endif

extern int ROOMEFFECT_COMET_SPARK_CTRL_CALLBACK(int mode, RoomCometSpark *spark);

/* Controller for the comet sparks: anchors on the actor, marks the primary
 * channel's first node, then every fourth frame fires a burst of ten sparks
 * along a random direction until the countdown runs out. */
int ROOMEFFECT_COMET_SPARK_CTRL_FUNC(int mode, RoomCometSparkAnchor *anchor) {
    GteShortVector direction;
    GteShortVector origin;
    RoomCometSpark *child;
    RoomSparkNode *node;
    int i;

    switch (mode) {
    case 0:
        anchor->count = 6;
        func_800CE870(D_800F32D0->pool, 0, anchor);
        if (D_800E2368->active) {
            if (D_800F32D0->pool) {
                node = *(RoomSparkNode **)D_800F32D0->pool;
                if (node) {
                    if (node->state[0] == 1) node->state[0] = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x24, 22,
                             ROOMEFFECT_COMET_SPARK_CTRL_CALLBACK);
    case 1:
        if ((D_800E27EC & 3) == 0) {
            if (anchor->count <= 0) return 2;
            func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0, &direction);
            direction.z = 0;
            direction.x = 0;
            direction.y += (func_80071A54() & 0x1FF) - 0x100;
            func_800CFB7C(&direction, (s16)(func_80071A54() % 1400 + 1500), &origin);
            origin.x += anchor->x;
            origin.y += anchor->y;
            origin.z += anchor->z;
            origin.y = D_800942EC.count;
            child = func_800CE610_comet(D_800F33E0->pool);
            if (child) {
                child->vx = origin.x;
                child->vy = origin.y;
                child->vz = origin.z;
                child->state = 0;
                child->timer = 0;
            }
            child = func_800CE610_comet(D_800F33E0->pool);
            if (child) {
                child->x = origin.x;
                child->y = origin.y;
                child->z = origin.z;
                child->state = 1;
                child->timer = 0;
            }
            for (i = 0; i < 8; i++) {
                child = func_800CE610_comet(D_800F33E0->pool);
                if (child) {
                    child->x = origin.x;
                    child->y = origin.y;
                    child->z = origin.z;
                    child->vx = func_80071A54() % 128 - 64;
                    child->vy = -(func_80071A54() % 128);
                    child->vz = func_80071A54() % 128 - 64;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            anchor->count--;
        }
        if (anchor->count > 0) break;
        return 2;
    case 2:
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}

#undef ROOMEFFECT_COMET_SPARK_CTRL_FUNC
#undef ROOMEFFECT_COMET_SPARK_CTRL_CALLBACK
