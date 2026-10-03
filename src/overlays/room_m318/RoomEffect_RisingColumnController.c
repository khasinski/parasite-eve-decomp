/* MASPSX_FLAGS: --expand-div */
#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

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
