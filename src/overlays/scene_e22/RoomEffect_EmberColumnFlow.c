#include "pe1/scene_e22_ember_burst.h"
#include "pe1/gte.h"

/* Rising ember particle: an ember drifts on a slowly turning circle while
 * it bounces off the floor (state 0), sparks coast to rest (states 1 and
 * 2); each draws as a turning glow. */
int func_8019485C(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin;
    RenderColor color = D_8018F204;
    int fall;
    int bounce;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y;
            p->heading.y = fall;
            if (p->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            p->x += func_80077CF4((s16)p->timer << 8) * 40 / 4096;
            p->y += func_80077DC4((s16)p->timer << 8) * 40 / 4096;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = func_80077DC4((s16)p->timer << 5) / 2 + 0x800;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                spin.x = 0;
                spin.y = 0;
                D_800F3368.tpage = tpage;
            }
            spin.z = (s16)p->timer * 32;
            spin.flags = 0;
            func_800CF3AC(D_80199388, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size, size,
                          (s16)D_800F3368.parameter02 + 0x64,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                         : palette + 4),
                          1, 0x80, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            size = func_80077CF4((s16)p->timer << 6) + 0x800;
            glow = func_80077DC4((s16)p->timer * 1204 / 16) / 128;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                spin.x = 0;
                spin.y = 0;
                D_800F3368.tpage = tpage;
            }
            spin.z = (s16)p->timer * 60;
            spin.flags = 0;
            func_800CF3AC(D_80199388, &color, ((s16)p->timer << 5) / 12);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size * 2, size * 2, 0x40,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                         : palette + 2),
                          1, glow, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11EA.index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                spin.x = 0;
                spin.y = 0;
                D_800F3368.tpage = tpage;
            }
            spin.z = (s16)p->timer * 60;
            spin.flags = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, 0x1800, 0x1800,
                          (s16)D_800F3368.parameter02 * (s16)p->timer + 0x80,
                          func_80077AA4(0, palette), 1, 0x40, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

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
