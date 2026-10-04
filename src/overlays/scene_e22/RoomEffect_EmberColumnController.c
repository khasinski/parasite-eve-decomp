#include "pe1/scene_e22_ember_burst.h"
#include "pe1/gte.h"

/* Ember column controller: rides the room actor and for 32 frames shakes
 * the camera while it sprays rising embers, coasting sparks and (on odd
 * frames) slow sparks; at frame 29 it hands the actor to the scene
 * script. It draws a flare, a halo and a floor ring while it burns. */
int func_80194F60(int mode, GteShortVector *anchor) {
    GteRotation spin = D_8018F1FC;
    GteShortVector position;
    RenderColor color = D_8018F210;
    RenderColor shade;
    RoomOrbitTrailParticle *child;
    SceneE22EmberObjectChannel *channel;
    SceneE22EmberSlot *pool;
    SceneE22EmberObject *object;
    int glow;
    int size;
    int i;

    switch (mode) {
    case 0:
        func_800D3F64(0x5DC, func_800D3FD8());
        func_800D3F64(0x5DD, 0x80);
        func_800CE870((char *)RoomMain_ActorPtr, 0, (s16 *)anchor);
        if (D_800E2368->active) {
            pool = D_800F32D0->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x3C, func_8019485C);
    case 1:
        if (D_800E27EC < 0x1C)
            func_80020D50();
        if (D_800E27EC < 0x11) {
            for (i = 0; i < 2; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = anchor->x;
                    child->y = anchor->y;
                    child->z = anchor->z;
                    child->heading.x = func_80071A54() % 64 - 0x20;
                    child->heading.y = func_80071A54() % 64 - 0x20;
                    child->heading.z = func_80071A54() % 64 - 0x20;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 0x21) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = anchor->x;
                child->y = anchor->y;
                child->z = anchor->z;
                child->heading.x = func_80071A54() % 40 - 20;
                child->heading.y = func_80071A54() % 32 - 16;
                child->heading.z = func_80071A54() % 40 - 20;
                child->state = 1;
                child->timer = 0;
            }
            if (D_800E27EC & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = anchor->x;
                    child->y = anchor->y;
                    child->z = anchor->z;
                    child->heading.x = func_80071A54() % 48 - 24;
                    child->heading.y = func_80071A54() % 48 - 24;
                    child->heading.z = func_80071A54() % 48 - 24;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 0x1C)
            func_80020DD0();
        if (D_800E27EC == 0x1D && D_800E2368->active) {
            channel = D_800F32D0;
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                (*RoomMain_ActorPtr)->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (D_800E27EC < 0x20) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 5;
            D_800F3368.depth = 0x20;
            D_800F3368.tpage = tpage;
        }
        if (D_800E27EC < 0x21) {
            int kind;
            int palette;
            glow = func_80077CF4(D_800E27EC << 6) / 32;
            if (D_800E27EC & 1)
                glow = glow * 15 / 16;
            size = func_80077CF4(D_800E27EC << 5) / 2 + 0x800;
            func_800CE870((char *)D_800F32D0->pool, 0, (s16 *)&position);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&position, 0, size * 3 / 2, size * 3 / 2, 0x46,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 11
                                                                         : palette + 7),
                          1, glow, 0);
            func_800D004C(&position, 800, 600, 0x10, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            size = func_80077CF4(D_800E27EC << 5);
            position.x = anchor->x;
            position.y = anchor->y;
            position.z = anchor->z;
            position.y = D_800942EC.y;
            func_800CF3AC(D_80199388, &shade, D_800E27EC);
            func_800D0728(&position, 600, 900, 0x14, &spin, size, size, 0, &shade,
                          glow / 2, 1);
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}
