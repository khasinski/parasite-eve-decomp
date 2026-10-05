#include "pe1/scene_e19_2_homing_burst.h"

/* Mode 0 anchors the burst on the room actor and wakes the actor's
 * object; mode 1 releases a homing particle on odd frames (twenty in all)
 * and reports 2 after eight frames; mode 2 draws the flash, the screen
 * tint and the glows that shrink over 32 frames. */
int func_801960C4(int mode, SceneHomingBurst *burst)
{
    GteShortVector position;
    GteRotation rotation = D_8018F210;
    GteRotation tilt = D_8018F1CC;
    RenderColor flashColor = D_8018F21C;
    RenderColor ringColor = D_8018F220;
    SceneHomingBurstChild *child;
    int fade;
    int scale;
    int time;

    switch (mode) {
    case 0:
        func_800CE8F0(D_800F32D0->pool, 0, &rotation, &burst->position);
        burst->timer = 0;
        burst->released = 0;
        if (D_800E2368->active) {
            SceneHomingBurstPool *pool = D_800F32D0->pool;
            if (pool) {
                SceneHomingBurstObject *object = pool->object;
                if (object) {
                    if (*object->status == 1) *object->status = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x20, 0x20, func_8019549C);
    case 1:
        burst->timer++;
        if (burst->released < 0x14 && (burst->timer & 1)) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->index = burst->released;
                child->state = 0;
                child->timer = 0;
            }
            burst->released++;
        }
        if (burst->timer < 8) break;
        return 2;
    case 2:
        position.x = burst->position.x;
        position.y = burst->position.y;
        position.z = burst->position.z;
        time = burst->timer;
        if (time < 0x19) {
            fade = 0x80 - (time << 7) / 24;
            scale = func_80077CF4((time << 10) / 24);
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800D0728(&position, 0x6A4, 0x9C4, 0x16, 0, scale, scale, 0, &flashColor,
                          fade, 1);
            time = burst->timer;
        }
        if (time < 0xD) {
            func_800D1AE0(&flashColor, 0x80 - (time << 7) / 12, 1, 8);
        }
        if (burst->timer < 0x21) {
            {
                int tpage;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                tpage = D_800E2850[D_800E11EA.value];
                D_800F3368.palette = 3;
                D_800F3368.depth = 0x32;
                D_800F3368.parameter06 = 0;
                D_800F3368.parameter0A = 0;
                D_800F3368.tpage = tpage;
            }
            {
                int ticks = burst->timer;
                fade = 0x80 - ticks * 4;
                tilt.z = ticks << 4;
            }
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&position, &tilt, 0x3000, 0x3000, 0x98,
                              func_80077AA4(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&position, &tilt, 0x1800, 0x1800, 0x98,
                              func_80077AA4(0x70, palette), 1, fade, 0);
            }
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&position, 0, 0x2000, 0x2000, 8,
                              func_80077AA4(0x90, palette), 1, fade / 2, 0);
            }
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            scale = (burst->timer << 5) + 0x1000;
            func_800D0728(&position, 0x2BC, 1000, 0x14, 0, scale, scale, 0, &ringColor,
                          fade / 2, 1);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA.value];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
