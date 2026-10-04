#include "pe1/scene_e22_ember_burst.h"
#include "pe1/gte.h"

/* Ember drift particle: embers drift and bounce off the floor while their
 * sideways speed decays (state 0 slowly, state 1 quickly while falling),
 * and a wide glow pulses in place (state 2). */
int func_80193940(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1F4;
    GteRotation turn = D_8018F1FC;
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
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y;
            p->heading.y = fall;
            if (p->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = func_80077CF4((s16)p->timer << 7) * 3 / 2;
            glow = func_80077CF4((s16)p->timer << 7) / 160;
            spin.z = -((s16)p->timer * 32);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11EA.index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_80199308, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size,
                          (s16)D_800F3368.parameter02 * ((s16)p->timer / 2) + 0x80,
                          func_80077AA4(0, palette), 2, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            size = func_80077DC4(((s16)p->timer << 10) / 24);
            spin.z = (s16)p->timer * 32;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA.index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_80199308, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size / 2, size / 2, 0x44,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 10
                                                                    : palette + 6),
                          1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            glow = func_80077CF4(((s16)p->timer << 11) / 24) / 32;
            size = func_80077DC4(((s16)p->timer << 10) / 24) / 2 + 0x800;
            turn.z = D_800E27EC << 6;
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
                int tpage = D_800E2850[D_800E11FA.index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_80199308, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &turn, size * 2, size * 2, 0x40,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                    : palette + 2),
                          1, glow / 2, &color);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Ember drift controller: rides the room actor and glows up for 16
 * frames, then for 32 frames shakes the camera while it sprays orbiting
 * sparks, embers on odd frames and smoke every sixth; at frame 25 it hands
 * the actor to the scene script; then it fades. While it glows it draws a
 * flickering halo and ring. */
int func_8019408C(int mode, SceneE22EmberBurst *burst) {
    RenderColor ringColor = D_8018F208;
    RenderColor haloColor = D_8018F20C;
    RoomOrbitTrailParticle *child;
    SceneE22EmberObjectChannel *channel;
    SceneE22EmberSlot *pool;
    SceneE22EmberObject *object;
    int glow;

    switch (mode) {
    case 0:
        burst->state = 0;
        burst->timer = 0;
        burst->glow = 0;
        func_800D3F64(0x5DB, func_800D3FD8());
        func_800CE870((char *)RoomMain_ActorPtr, 1, (s16 *)burst);
        burst->y = D_800942EC.y;
        if (D_800E2368->active) {
            pool = D_800F32D0->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28, func_80193940);
    case 1:
        switch (burst->state) {
        case 0:
            burst->timer++;
            func_80020D50();
            burst->glow = func_80077CF4(burst->timer << 6) / 32;
            if (burst->timer < 0x10) break;
            burst->state = 1;
            burst->timer = 0;
            break;
        case 1:
            burst->timer++;
            if (burst->timer < 0x18)
                func_80020D50();
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
            if (burst->timer == 0x18)
                func_80020DD0();
            if (burst->timer == 0x19 && D_800E2368->active) {
                channel = D_800F32D0;
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    (*RoomMain_ActorPtr)->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
            if (burst->timer < 0x20) break;
            burst->state = 2;
            burst->timer = 0;
            break;
        case 2:
            burst->timer++;
            burst->glow = func_80077DC4((burst->timer << 10) / 24) / 32;
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
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA.index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        if (burst->glow != 0) {
            RenderColor *color;
            int scale;
            glow = burst->glow;
            color = (D_800E27EC & 1) ? &ringColor : &haloColor;
            scale = 0x1000;
            D_800F3368.depth = 0x28;
            func_800D004C((GteShortVector *)burst, 400, 400, 0x10, 0, scale, scale,
                          color, 0, glow, 1);
            func_800D0728((GteShortVector *)burst, 500, 600, 0x18, 0, scale, scale, 0,
                          color, glow / 2, 3);
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
