#include "pe1/scene_e19_2_ember.h"
#include "pe1/gte.h"

/* Ember flare controller: rides the room actor and glows up for 16
 * frames, then for 32 frames sprays orbiting sparks, embers on odd frames
 * and flares every sixth; then it fades and, on its first fading frame,
 * hands the actor to the scene script. While it glows it draws two floor
 * glows, a halo and a ring. */
int func_801978EC(int mode, SceneE22EmberBurst *burst) {
    RenderColor color = D_8018F238;
    GteShortVector position;
    GteRotation spin;
    GteShortVector floorPosition;
    GteRotation floorSpin;
    RoomOrbitTrailParticle *child;
    SceneE22EmberObjectChannel *channel;
    SceneE22EmberSlot *pool;
    SceneE22EmberObject *object;
    void **soundSlot;
    int volume;
    int glow;
    s16 z;

    switch (mode) {
    case 0:
        burst->state = 0;
        burst->timer = 0;
        burst->glow = 0;
        if (D_800E2368->active) {
            pool = D_800F32D0->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        soundSlot = &D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            func_8006DF50(*soundSlot, 0x5E5, func_800D3FD8(), 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                func_8006DF50(*soundSlot, 0x5E6, 0x80, 0x80, volume);
        }
        func_800CE870((char *)RoomMain_ActorPtr, 1, (s16 *)burst);
        burst->y = D_800942EC.y;
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28, func_801972EC);
    case 1:
        if (burst->state < 2)
            func_80020D50();
        switch (burst->state) {
        case 0:
            burst->timer++;
            burst->glow = func_80077CF4(burst->timer << 6) / 32;
            if (burst->timer < 0x10) break;
            burst->state = 1;
            burst->timer = 0;
            break;
        case 1:
            burst->timer++;
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->heading.x = func_80071A54() % 50 - 25;
                child->heading.y = -(func_80071A54() % 32);
                child->heading.z = func_80071A54() % 50 - 25;
                child->state = 1;
                child->timer = 0;
            }
            if (burst->timer & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->y -= func_80071A54() & 0x1FF;
                    child->x += (func_80071A54() & 0x7F) - 0x40;
                    child->z += (func_80071A54() & 0x7F) - 0x40;
                    child->heading.x = func_80071A54() % 24 - 12;
                    child->heading.y = func_80071A54() % 24 - 12;
                    child->heading.z = func_80071A54() % 24 - 12;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (burst->timer % 6 == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (burst->timer < 0x20) return 0;
            func_80020DD0();
            burst->state = 2;
            burst->timer = 0;
            break;
        case 2:
            burst->timer++;
            burst->glow = func_80077DC4((burst->timer << 10) / 24) / 32;
            if (burst->timer == 1 && D_800E2368->active) {
                channel = D_800F32D0;
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    (*RoomMain_ActorPtr)->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
            if (burst->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        if (burst->glow != 0) {
            glow = burst->glow;
            if (burst->timer & 1)
                glow = glow * 15 / 16;
            if (D_800E27EC & 1)
                glow = glow * 2 / 3;
            spin.x = 0x400;
            spin.z = D_800E27EC << 3;
            D_800F3368.depth = 0;
            spin.y = 0;
            spin.flags = 1;
            {
                int kind = D_800F3368.palette;
                int palette;
                position.x = burst->x;
                z = burst->z;
                position.y = D_800942EC.y;
                position.z = z;
                palette = D_800E1204[kind];
                func_800CEE20(&position, &spin, 0x1000, 0x1000, 0,
                              func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                        : palette + 2),
                              1, glow, 0);
            }
            {
                int tpage = D_800E2850[D_800E11EA.index];
                D_800F3368.tpage = tpage;
                D_800F3368.parameter06 = 0;
                D_800F3368.palette = 3;
                floorSpin.x = 0x400;
                floorSpin.y = 0;
                floorSpin.z = D_800E27EC << 5;
                floorSpin.flags = 1;
            }
            floorPosition.x = burst->x;
            z = burst->z;
            floorPosition.y = D_800942EC.y;
            floorPosition.z = z;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&floorPosition, &floorSpin, 0x1800, 0x1800, 0x40,
                              func_80077AA4(0x10, palette), 1, glow, 0);
            }
            D_800F3368.depth = 0x28;
            func_800D004C((GteShortVector *)burst, 400, 400, 0x10, 0, 0x1000, 0x1000,
                          &color, 0, glow, 1);
            func_800D0728((GteShortVector *)burst, 500, 600, 0x18, 0, 0x1000, 0x1000, 0,
                          &color, glow, 3);
        }
        {
            int tpage = D_800E2850[D_800E11EA.index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 4;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
