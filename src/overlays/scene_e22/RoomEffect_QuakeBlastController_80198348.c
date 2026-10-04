/* MASPSX_FLAGS: --expand-div */
#include "pe1/scene_e22_quake_blast.h"

/* Quake blast controller: on frame 6 it lights the actor if it stands in
 * reach, sprays sparks for six frames and ends after 40. It flashes the
 * screen, then draws a floor glow, a halo, a ring and three shock models
 * that spread from the blast. */
int func_80198348(int mode, SceneE22QuakeBlast *fx, SceneE22QuakeTarget *target) {
    GteShortVector position;
    GteRotation tilt = D_8018F1FC;
    GteRotation rotation;
    SceneE22QuakeColor color;
    RenderColor flash = D_8018F228;
    GteShortVector floor;
    GteRotation flat;
    RoomOrbitTrailParticle *child;
    SceneE22QuakeObjectChannel *channel;
    int glow;
    int phase;
    int scale;
    int i;

    switch (mode) {
    case 0:
        fx->state = 0;
        fx->timer = 0;
        if (D_800B0E64.channel) {
            func_8006DF50(D_800B0E64.channel, 0x5DA, func_800D3FD8(), 0x80, 0x7F);
        }
        D_80199508 = func_8006E498(D_800B0E64.channel, 0xC58C4704);
        func_800C6D5C(D_80199508, 0, 0);
        D_8019950C = func_8006E498(D_800B0E64.channel, 0xC5CC4704);
        func_800C6D5C(D_8019950C, 0, 0);
        fx->position.x = target->x;
        fx->position.y = target->y;
        fx->position.z = target->z;
        return func_800CE560(D_800F33E0->pool, 0xC, 0x20, func_801981B0);
    case 1:
        fx->timer++;
        if (fx->timer == 6 && func_800C6B90(fx, target->radius) && D_800E2368->active) {
            channel = D_800F32D0;
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                (*RoomMain_ActorPtr)->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x21000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (fx->timer < 7) {
            phase = 0x1000;
            for (i = 0; i < 4; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = fx->position.x;
                    child->y = fx->position.y;
                    child->z = fx->position.z;
                    child->radius = (func_80071A54() & 0xFFF) + 0x800;
                    child->y -= func_80071A54() & 0x7F;
                    child->x += func_80071A54() % phase - phase / 2;
                    child->z += func_80071A54() % phase - phase / 2;
                    child->heading.x = 0;
                    child->heading.y = 0;
                }
            }
        }
        if (fx->timer < 0x28) break;
        return 1;
    case 2:
        position.x = fx->position.x;
        position.y = fx->position.y;
        position.z = fx->position.z;
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x29;
        if (fx->timer < 4) {
            color.word = 0xFFFFFF;
            func_800D1AE0(&color.color, 0x6E, 2, 8);
        } else if (fx->timer < 6) {
            func_800D1AE0(&flash, 0x80, 1, 8);
        } else if (fx->timer < 0xD) {
            func_800D1AE0(&flash, func_80077DC4(((fx->timer - 6) << 10) / 6) / 64, 1, 8);
        }
        glow = func_80077DC4((fx->timer << 10) / 40) / 32;
        if ((u16)fx->timer & 1)
            glow = glow * 3 / 4;
        {
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11EA[8]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            flat.x = 0x400;
            flat.y = 0;
            flat.z = 0;
            flat.flags = 1;
            floor.x = position.x;
            floor.z = position.z;
            floor.y = D_800942EC.y;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&floor, &flat, 0x2000, 0x2000, 0x64,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                         : palette + 4),
                          1, glow, 0);
        }
        func_800CF3AC(D_8019948C, &color.color, fx->timer);
        func_800D004C(&position, 0x4B0, 0x5DC, 0x10, 0, 0x1000, 0x1000, &color.color, 0,
                      glow, 1);
        phase = (fx->timer << 10) / 40;
        scale = func_80077CF4(phase);
        func_800D0728(&position, 0xBB8, 0x1770, 0x12, &tilt, scale, scale, 0, &color.color,
                      glow, 1);
        D_800F3368.depth = 4;
        glow = func_80077DC4(phase) / 32;
        scale = func_80077CF4(phase) + 0x800;
        {
            SceneE22QuakeMatrix matrix;
            SceneE22QuakeScale shape;
            int kind;
            int palette;
            int page;
            rotation.x = 0;
            rotation.y = func_80077CF4(phase);
            rotation.z = 0;
            {
                int tpage = D_800E2850[D_800E11EA[0]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800C6EC0(page, func_80077AA4(0x10, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &matrix);
            shape.x = scale;
            shape.y = scale * 2;
            shape.z = scale;
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            func_80078CC4(&matrix, &shape);
            func_800C6EF8(D_80199508);
            func_800C6FA0(D_80199508, (u16)(glow * 3 / 2));
            func_800C71E4(D_80199508, &matrix);
            func_800C6F4C(D_80199508);
        }
        scale = scale / 2;
        {
            SceneE22QuakeMatrix matrix;
            SceneE22QuakeScale shape;
            int kind;
            int palette;
            int page;
            rotation.x = 0;
            rotation.y = func_80077CF4(phase) * 2;
            rotation.z = 0;
            {
                int tpage = D_800E2850[D_800E11EA[0]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800C6EC0(page, func_80077AA4(0x10, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &matrix);
            shape.x = scale;
            shape.y = scale * 8;
            shape.z = scale;
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            func_80078CC4(&matrix, &shape);
            func_800C6EF8(D_80199508);
            func_800C6FA0(D_80199508, (u16)glow);
            func_800C71E4(D_80199508, &matrix);
            func_800C6F4C(D_80199508);
        }
        if (fx->timer < 0x19) {
            SceneE22QuakeMatrix matrix;
            SceneE22QuakeScale shape;
            int kind;
            int palette;
            int page;
            phase = (fx->timer << 10) / 24;
            glow = func_80077DC4(phase) / 32;
            scale = func_80077DC4(phase);
            rotation.x = 0;
            rotation.y = fx->timer * 40;
            rotation.z = 0;
            {
                int tpage = D_800E2850[D_800E11EA[0]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800C6EC0(page, func_80077AA4(0x30, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &matrix);
            shape.x = scale;
            shape.y = 0x1000;
            shape.z = scale;
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            func_80078CC4(&matrix, &shape);
            func_800C6EF8(D_8019950C);
            func_800C6FA0(D_8019950C, (u16)glow);
            func_800C71E4(D_8019950C, &matrix);
            func_800C6F4C(D_8019950C);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        {
            int tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
