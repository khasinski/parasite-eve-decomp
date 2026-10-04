/* MASPSX_FLAGS: --expand-div */
#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

/* Rising column particle: rings pulse at the column centre while sparks
 * drift and bounce on the floor; both draw with the column's glow track. */
int func_80195190(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F208;
    RenderColor color;
    int fall;
    int bounce;
    int angle;
    int size;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            size = func_80077CF4(angle) * 5 / 2;
            angle = func_80077DC4(angle) / 32;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            spin.z += D_800E27EC << 5;
            {
                int tpage = D_800E2850[D_800E11F8];
                D_800F3368.palette = 2;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_801996EC, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&D_80199924, &spin, size, size, 0x44,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                    : palette + 4),
                          1, angle, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = func_80077DC4(angle) / 2 + 0x800;
            angle = func_80077DC4(angle) / 32;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            {
                int tpage = D_800E2850[D_800E11E8];
                D_800F3368.palette = 2;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_801996EC, &color, (s16)p->timer * 2);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, 0, size / 2, size / 2,
                          (s16)D_800F3368.parameter02 * 2 + 0xD8,
                          GetClut(0x20, palette), 1, angle, &color);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Rising column: follows the actor while its height eases from the floor
 * over the configured duration, shedding a falling spark every frame and
 * a ring every fourth; the draw publishes it as the column centre. */
int func_801955E4(int mode, RoomRisingColumn *column,
                  RoomRisingColumnParams *params) {
    RoomOrbitTrailParticle *child;
    int rise;

    switch (mode) {
    case 0:
        column->counter = 0;
        column->timer = 0;
        func_800D3F64(0x5EA, func_800D3FD8());
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20, func_80195190);
    case 1:
        column->timer++;
        func_800CE870((char *)D_800F32D0->pool, 0, (s16 *)column);
        column->y = D_800942EC.y - params->offset;
        rise = params->height * column->timer / params->duration;
        if (params->descend)
            column->y -= rise;
        else
            column->y -= params->height - rise;
        if (column->timer <= params->duration) {
            if ((column->timer & 3) == 0) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->state = 0;
                    child->timer = 0;
                }
            }
            child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = column->x;
                child->y = column->y;
                child->z = column->z;
                child->x += (func_80071A54() & 0xFF) - 0x80;
                child->z += (func_80071A54() & 0xFF) - 0x80;
                child->heading.x = func_80071A54() % 32 - 0x10;
                /* The vertical heading spends a random draw on a zero range. */
                child->heading.y = func_80071A54() % 1;
                child->heading.z = func_80071A54() % 32 - 0x10;
                child->state = 1;
                child->timer = 0;
            }
        }
        if (column->timer < params->duration + 8) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_80199924.x = column->x;
        D_80199924.y = column->y;
        D_80199924.z = column->z;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
