#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

/* Fan sweep spark: circles the published anchor while it rises, dropping a
 * trail point now and then; it swells for 12 frames, holds for 12, then
 * fades while its radius grows. */
int func_80192718(int mode, RoomFanSweepSpark *spark) {
    GteShortVector position;
    GteRotation spin;
    RenderColor color = D_8018F1F0;
    GteShortVector *point;
    int size;
    int alpha;

    switch (mode) {
    case 1:
        spark->timer++;
        spark->y = D_800E27EC * 700 / 36 - 700 + D_801998F8.y;
        spark->x = D_801998F8.x + func_80077DC4((s16)spark->angle) * spark->radius / 4096;
        spark->z = D_801998F8.z + func_80077CF4((s16)spark->angle) * spark->radius / 4096;
        spark->angle += 0x80;
        if (!(func_80071A54() & 7)) {
            point = (GteShortVector *)func_800CE610(D_80199900);
            if (point) {
                point->x = spark->x;
                point->y = spark->y;
                point->z = spark->z;
            }
        }
        if (D_800E27EC < 0x24) break;
        return 1;
    case 2:
        switch (spark->state) {
        case 0:
            size = func_80077CF4(((s16)spark->timer << 10) / 12);
            alpha = 0x40;
            if ((s16)spark->timer < 12) break;
            spark->timer = 0;
            spark->state = 1;
            break;
        case 1:
            size = 0x1000;
            alpha = 0x40;
            if ((s16)spark->timer < 12) break;
            spark->timer = 0;
            spark->state = 2;
            break;
        default:
            size = 0x1000;
            alpha = func_80077DC4(((s16)spark->timer << 10) / 12) / 64;
            spark->radius += (s16)spark->timer * 4;
            break;
        }
        position.x = spark->x;
        position.y = spark->y;
        position.z = spark->z;
        size = size * 3 / 2;
        spin.x = 0;
        spin.y = 0;
        spin.z = D_800E27EC << 7;
        spin.flags = 0;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&position, &spin, size, size, 0x24,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 9 : palette + 5),
                          1, alpha, &color);
        }
        break;
    }
    return 0;
}


/* Fan sweep: attaches to the actor named by its parameters at floor level,
 * plays its sounds and opens a spark pool plus a trail pool; for 32 frames
 * every other frame it launches a spark on an angle that turns by a third
 * of a circle, updates the trail pool for 70 frames and each draw
 * publishes its anchor for the trail. */
int func_80192A7C(int mode, RoomFanSweep *sweep, RoomEmberBurstParams *params) {
    RoomFanSweepSpark *spark;
    int handles;

    switch (mode) {
    case 0:
        sweep->angle = func_80071A54();
        RoomEffect_AttachToActor(params->subId, params->typeId, sweep);
        sweep->y = D_800942EC.y;
        func_800D3F64(0x5F3, func_800D3FD8());
        func_800D3F64(0x5F4, 0x80);
        handles = func_800CE560(D_800F33E0->pool, 0x10, 0x18, func_80192718);
        handles += func_800CE5AC(&sweep->pool, handles, 8, 0x12, func_80192620);
        return handles;
    case 1:
        if (D_800E27EC < 0x20 && (D_800E27EC & 1)) {
            spark = (RoomFanSweepSpark *)func_800CE610(D_800F33E0->pool);
            if (spark) {
                spark->angle = sweep->angle;
                spark->radius = 0xC8;
                spark->state = 0;
                spark->timer = 0;
                sweep->angle -= 0x555;
            }
        }
        if (D_800E27EC >= 0x46) return 1;
        func_800CE688(sweep->pool);
        D_80199900 = sweep->pool;
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_800F3368.depth = 8;
        func_800CE78C(sweep->pool);
        D_801998F8.x = sweep->x;
        D_801998F8.y = sweep->y;
        D_801998F8.z = sweep->z;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
