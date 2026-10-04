#include "pe1/scene_e22_ember_burst.h"
#include "pe1/gte.h"

/* Twist model controller: a model bolt that rides an actor joint along its
 * heading; it waits 16 frames, then for 16 frames advances and sheds a
 * twisting trail particle per frame. It draws as a spinning model, plus
 * glows and flares while it advances. */
int func_80196554(int mode, SceneE22TwistModel *bolt) {
    RenderColor color = D_8018F218;
    GteShortVector position;
    GteShortVector step;
    GteRotation rotation;
    GteRotation spin = D_8018F1F4;
    GteShortVector offset = D_8018F1E0;
    RoomOrbitTrailParticle *child;
    int glow;
    int angle;
    int modelScale;

    switch (mode) {
    case 0:
        bolt->state = 0;
        bolt->timer = 0;
        bolt->reserved1C = 0;
        D_80199500 = func_8006E498(D_800B0E64.channel, 0xC54C4704);
        func_800C6D5C(D_80199500, 0, 0);
        func_800CE8F0(D_800F32D0->pool, 0, &offset, &bolt->position);
        bolt->origin.x = bolt->position.x;
        bolt->origin.y = bolt->position.y;
        bolt->origin.z = bolt->position.z;
        func_800CE9D4(D_800F32D0->pool, 0, &bolt->heading);
        bolt->heading.x -= 0x220;
        bolt->heading.y -= 0x80;
        return func_800CE560(D_800F33E0->pool, 0x14, 0x18, func_801962FC);
    case 1:
        func_800CFB7C(&bolt->heading, 0x140, &position);
        position.x += bolt->position.x;
        position.y += bolt->position.y;
        position.z += bolt->position.z;
        switch (bolt->state) {
        case 0:
            bolt->timer++;
            if (bolt->timer < 0x10) break;
            bolt->state = 1;
            bolt->timer = 0;
            break;
        case 1:
            bolt->timer++;
            func_800CFB7C(&bolt->heading, 0xA0, &step);
            bolt->position.x += step.x;
            bolt->position.y += step.y;
            bolt->position.z += step.z;
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->radius = (func_80071A54() & 0xFF) + 0x180;
                child->heading.x = bolt->heading.x;
                child->heading.y = bolt->heading.y;
                child->heading.z = bolt->heading.z;
                child->heading.pad = func_80071A54();
                child->state = 0;
                child->timer = 0;
            }
            if (bolt->timer < 0x10) break;
            bolt->state = 2;
            bolt->timer = 0;
            break;
        case 2:
            return 2;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        func_800CFB7C(&bolt->heading, 0x140, &position);
        position.x += bolt->position.x;
        position.y += bolt->position.y;
        position.z += bolt->position.z;
        modelScale = 0x2AA;
        switch (bolt->state) {
        case 0: {
            SceneE22ModelMatrix matrix;
            SceneE22ModelScale scale;
            glow = func_80077CF4(bolt->timer << 6) / 32;
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
            break;
        }
        case 1: {
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
                func_800CEE20(&position, &spin, 0x1000, 0x1000, 4,
                              func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 7
                                                                             : palette + 3),
                              1, glow / 2, 0);
            }
            modelScale = func_80077CF4(angle) + 0x1000;
            func_800D004C(&position, 0x44C, 0x5DC, 0x20, 0, modelScale, modelScale, &color, 0,
                          glow, 1);
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 8;
                else palette += 4;
                func_800D3BC8(&position, 0x1000, 0x1000, (s16)D_800F3368.parameter02 + 0x64,
                              func_80077AA4(0, palette), 1, glow, 0, 0x600);
            }
            {
                int kind;
                int palette;
                {
                    int tpage = D_800E2850[D_800E11EA.index];
                    D_800F3368.palette = 3;
                    D_800F3368.parameter06 = 0;
                    D_800F3368.parameter00 = 0x20;
                    D_800F3368.parameter02 = 2;
                    D_800F3368.extent_x = 0x20;
                    D_800F3368.extent_y = 0x20;
                    D_800F3368.tpage = tpage;
                }
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800D3BC8(&position, 0x1800, 0x1800, 2,
                              func_80077AA4(0x40, palette), 1, glow, 0, 0xA00);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800D3BC8(&position, 0x1800, 0x1800, 4,
                              func_80077AA4(0x50, palette), 1, glow, 0, 0xE00);
            }
            break;
        }
        case 2:
            break;
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
