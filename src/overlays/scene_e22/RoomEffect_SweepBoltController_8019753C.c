#include "pe1/scene_e22_ember_burst.h"
#include "pe1/gte.h"

/* Sweep bolt controller: the bolt swings out from its origin over sixteen
 * frames shedding twist beam particles, then hovers for 24 frames shaking
 * the screen and spraying sparks. It draws as a spinning model with a glow,
 * a halo and a ring. */
int func_8019753C(int mode, SceneE22SweepBolt *bolt, GteShortVector *origin) {
    RenderColor color = D_8018F218;
    RenderColor haloColor = D_8018F224;
    GteShortVector tip;
    GteRotation rotation;
    GteRotation spin = D_8018F1F4;
    GteRotation tilt = D_8018F1FC;
    RoomOrbitTrailParticle *child;
    int glow;
    int angle;
    int modelScale;
    int i;

    switch (mode) {
    case 0:
        bolt->state = 0;
        bolt->timer = 0;
        bolt->reserved1C = 0;
        bolt->sweep = 0;
        D_80199504 = 0;
        D_80199500 = func_8006E498(D_800B0E64.channel, 0xC54C4704);
        func_800C6D5C(D_80199500, 0, 0);
        if (D_800E2368->active) {
            SceneE22EmberSlot *pool = D_800F32D0->pool;
            if (pool) {
                SceneE22EmberObject *object = pool->object;
                if (object) {
                    if (*object->status == 1) *object->status = 2;
                }
            }
        }
        bolt->origin.x = origin->x;
        bolt->origin.y = origin->y;
        bolt->origin.z = origin->z;
        bolt->heading.x = -0x320;
        bolt->heading.y = -0x200;
        bolt->heading.z = 0;
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20, func_8019702C);
    case 1:
        func_800CFB7C(&bolt->heading, -((0x10 - bolt->sweep) * 320), &bolt->position);
        bolt->position.x += bolt->origin.x;
        bolt->position.y += bolt->origin.y;
        bolt->position.z += bolt->origin.z;
        func_800CFB7C(&bolt->heading, 0x140, &tip);
        tip.x += bolt->position.x;
        tip.y += bolt->position.y;
        tip.z += bolt->position.z;
        switch (bolt->state) {
        case 0:
            bolt->timer++;
            bolt->sweep++;
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = tip.x;
                child->y = tip.y;
                child->z = tip.z;
                child->radius = (func_80071A54() & 0xFF) + 0x180;
                child->heading.x = bolt->heading.x;
                child->heading.y = bolt->heading.y;
                child->heading.z = bolt->heading.z;
                child->heading.pad = func_80071A54();
                child->state = 0;
                child->timer = 0;
            }
            if (bolt->timer < 0x10) break;
            bolt->state = 1;
            bolt->timer = 0;
            if (D_800B0E64.channel) {
                func_8006DF50(D_800B0E64.channel, 0x5D8, func_800D3FD8(), 0x80, 0x7F);
                if (D_800B0E64.channel) {
                    func_8006DF50(D_800B0E64.channel, 0x5D9, 0x80, 0x80, 0x7F);
                }
            }
            break;
        case 1:
            bolt->timer++;
            angle = (bolt->timer << 10) / 24;
            func_80020D50();
            if (bolt->timer < 9) func_800D1D24(2, 8, bolt->timer);
            bolt->position.x = bolt->origin.x;
            bolt->position.y = bolt->origin.y;
            bolt->position.z = bolt->origin.z;
            if (D_800E27EC & 1) bolt->position.y += func_80077DC4(angle) / 512;
            else bolt->position.y -= func_80077DC4(angle) / 512;
            if (bolt->timer < 0xD) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = bolt->origin.x;
                    child->y = bolt->origin.y;
                    child->z = bolt->origin.z;
                    child->radius = (func_80071A54() & 0x3FF) + 0x7D0;
                    child->heading.x = 0;
                    child->heading.y = func_80071A54();
                    child->heading.z = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if ((u16)(bolt->timer - 4) < 5) {
                for (i = 0; i < 2; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = bolt->origin.x;
                        child->y = bolt->origin.y;
                        child->z = bolt->origin.z;
                        child->heading.x = func_80071A54() % 768 + 0x100;
                        child->heading.y = func_80071A54();
                        child->heading.z = func_80071A54();
                        child->state = 2;
                        child->timer = 0;
                    }
                }
            }
            if (bolt->timer < 0x18) break;
            func_80020DD0();
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        func_800CFB7C(&bolt->heading, 0x140, &tip);
        tip.x += bolt->position.x;
        tip.y += bolt->position.y;
        tip.z += bolt->position.z;
        modelScale = 0x2AA;
        switch (bolt->state) {
        case 0: {
            SceneE22ModelMatrix matrix;
            SceneE22ModelScale scale;
            angle = bolt->timer << 6;
            glow = (func_80077DC4(angle) / 2 + 0x800) / 32;
            if (D_800E27EC & 1)
                glow = glow * 3 / 4;
            {
                int kind;
                int palette;
                int page;
                u16 *slot = &D_800E11FA.index;
                int tpage = D_800E2850[*slot];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
                page = (u16)(D_800E2850[*slot] | func_80077A64(0, 1, 0, 0));
                kind = D_800F336C;
                palette = D_800E1204[kind];
                func_800C6EC0(page, func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 7
                                                                             : palette + 3));
            }
            func_800C6ED8(1);
            rotation.x = bolt->heading.x;
            rotation.y = bolt->heading.y;
            rotation.z = bolt->heading.z;
            rotation.x += 0x800;
            func_80079754(&rotation, &matrix);
            matrix.t[0] = bolt->position.x;
            matrix.t[1] = bolt->position.y;
            matrix.t[2] = bolt->position.z;
            scale.x = modelScale;
            scale.y = modelScale;
            scale.z = modelScale;
            func_80078CC4(&matrix, &scale);
            func_800C6EF8(D_80199500);
            func_800C6FA0(D_80199500, (u16)glow);
            func_800C71E4(D_80199500, &matrix);
            func_800C6F4C(D_80199500);
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                {
                    int tpage = D_800E2850[D_800E11FA.index];
                    D_800F3368.palette = 3;
                    D_800F3368.parameter06 = 1;
                    D_800F3368.parameter0A = 0;
                    D_800F3368.depth = 0x28;
                    D_800F3368.tpage = tpage;
                }
                spin.z = bolt->timer * 32;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                func_800CEE20(&tip, &spin, 0x1000, 0x1000, 4,
                              func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 7
                                                                             : palette + 3),
                              1, glow / 2, 0);
            }
            modelScale = func_80077CF4(angle) + 0x1000;
            func_800D004C(&tip, 0x44C, 0x5DC, 0x20, 0, modelScale, modelScale, &color, 0,
                          glow, 1);
            break;
        }
        case 1: {
            SceneE22ModelMatrix matrix;
            SceneE22ModelScale scale;
            angle = (bolt->timer << 10) / 24;
            glow = func_80077DC4(angle) / 32;
            if (D_800E27EC & 1)
                glow = glow * 3 / 4;
            {
                int kind;
                int palette;
                int page;
                u16 *slot = &D_800E11FA.index;
                int tpage = D_800E2850[*slot];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
                page = (u16)(D_800E2850[*slot] | func_80077A64(0, 1, 0, 0));
                kind = D_800F336C;
                palette = D_800E1204[kind];
                func_800C6EC0(page, func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 7
                                                                             : palette + 3));
            }
            func_800C6ED8(1);
            rotation.x = bolt->heading.x;
            rotation.y = bolt->heading.y;
            rotation.z = bolt->heading.z;
            rotation.x += 0x800;
            func_80079754(&rotation, &matrix);
            matrix.t[0] = bolt->position.x;
            matrix.t[1] = bolt->position.y;
            matrix.t[2] = bolt->position.z;
            scale.x = modelScale;
            scale.y = modelScale;
            scale.z = modelScale;
            func_80078CC4(&matrix, &scale);
            func_800C6EF8(D_80199500);
            func_800C6FA0(D_80199500, (u16)glow);
            func_800C71E4(D_80199500, &matrix);
            func_800C6F4C(D_80199500);
            modelScale = func_80077DC4(angle) + 0x1000;
            spin.z = bolt->timer * 32;
            func_800D004C(&bolt->origin, 1000, 0x32, 8, &spin, modelScale, modelScale,
                          &haloColor, 0, glow, 1);
            modelScale = func_80077CF4(angle);
            tilt.z = -(bolt->timer * 32);
            func_800D0728(&bolt->origin, 0x7D0, 0xFA0, 0x10, &tilt, modelScale, modelScale, 0,
                          &color, glow, 1);
            break;
        }
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
