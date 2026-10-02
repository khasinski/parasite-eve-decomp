#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_JITTER_SPARK_CTRL_FUNC
#error "ROOMEFFECT_JITTER_SPARK_CTRL_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_JITTER_SPARK_CTRL_CALLBACK
#error "ROOMEFFECT_JITTER_SPARK_CTRL_CALLBACK must name the particle callback"
#endif

extern int ROOMEFFECT_JITTER_SPARK_CTRL_CALLBACK(int mode, RoomDampedSpark *spark);

/* Controller that bursts a ring of jittering sparks on the first frame,
 * keeps spawning pairs for the first 32 frames, plays the two burst sounds
 * on creation and configures the sprite palette. */
int ROOMEFFECT_JITTER_SPARK_CTRL_FUNC(int mode, GteShortVector *target,
                                      RoomSparkRingParams *params) {
    RoomDampedSpark *child;
    int radius;
    int angle;
    int reach;
    int handle;
    void **sound;
    void *current;
    int volume;
    int i;

    switch (mode) {
    case 0:
        sound = &D_800B0E64;
        current = *sound;
        if (current != 0) {
            volume = 0x7F;
            handle = func_800D3FD8();
            func_8006DF50(*sound, 0x5FA, handle, 0x80, volume);
            current = *sound;
            /* Same empty barrier as RoomLib_SpawnRandomEffect: keeps the
             * re-read channel in a register for the second test. */
            asm("" : : "r"(current) : "memory");
            if (current != 0) {
                func_8006DF50(*sound, 0x5FB, 0x80, 0x80, volume);
            }
        }
        target->x = params->x;
        target->y = params->y;
        target->z = params->z;
        target->y = g_FrameCount16.count;
        return func_800CE560(D_800F33E0->pool, 20, 60, ROOMEFFECT_JITTER_SPARK_CTRL_CALLBACK);
    case 1:
        radius = params->radius;
        if (D_800E27EC == 1) {
            for (i = 0; i < 20; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->x;
                    child->y = target->y;
                    child->z = target->z;
                    angle = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(angle) * reach / 4096;
                    child->z += rsin(angle) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 64 - 32;
                    child->vy = -(func_80071A54() % 44);
                    child->vz = func_80071A54() % 64 - 32;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            for (i = 0; i < 16; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->x;
                    child->y = target->y;
                    child->z = target->z;
                    angle = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(angle) * reach / 4096;
                    child->z += rsin(angle) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 128 - 64;
                    child->vy = -(func_80071A54() % 80);
                    child->vz = func_80071A54() % 128 - 64;
                    child->state = (func_80071A54() & 1) + 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 33) {
            if (D_800E27EC & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->x;
                    child->y = target->y;
                    child->z = target->z;
                    angle = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(angle) * reach / 4096;
                    child->z += rsin(angle) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 70 - 35;
                    child->vy = -(func_80071A54() % 44);
                    child->vz = func_80071A54() % 70 - 35;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = target->x;
                child->y = target->y;
                child->z = target->z;
                angle = func_80071A54();
                reach = radius * (func_80071A54() & 0xFF) / 256;
                child->x += rcos(angle) * reach / 4096;
                child->z += rsin(angle) * reach / 4096;
                child->angle = func_80071A54();
                child->vx = func_80071A54() % 80 - 40;
                child->vy = -(func_80071A54() % 49);
                child->vz = func_80071A54() % 80 - 40;
                child->state = (func_80071A54() & 1) + 1;
                child->timer = 0;
            }
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2: {
        int index = D_800E11EA;
        int tpage;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        tpage = D_800E2850[index];
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

#undef ROOMEFFECT_JITTER_SPARK_CTRL_FUNC
#undef ROOMEFFECT_JITTER_SPARK_CTRL_CALLBACK
