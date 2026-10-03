/* MASPSX_FLAGS: --expand-div */
#include "pe1/room_m349_effects.h"

/* Spawns flare sparks on a ring around the target every `period` frames
 * (and a second kind half a period later) until `lastFrame`, scatters
 * glints around a model joint on random even frames, stops the looping
 * sound 16 frames after the ring ends and draws two joint glows. */
int func_8018F7C4(int mode, RoomM349FlareTarget *target,
                  RoomM349FlareParams *params) {
    GteShortVector joint;
    GteShortVector anchor;
    GteRotation rotation = D_8018F000;
    GteRotation rotation2 = D_8018F008;
    RenderColor color = D_8018EFFC;
    RoomM349FlareSpark *child;
    int radius;
    int period;
    int reach;
    int size;
    /* Ring angle while spawning, glow fade while drawing: one variable,
     * as retail keeps both in the same saved register. */
    int phase;

    switch (mode) {
    case 0:
        target->position.x = params->x;
        target->position.y = params->y;
        target->position.z = params->z;
        target->position.y = D_800942EC.count;
        return func_800CE560(D_800F33E0->pool, 20, 70, func_8018F010);
    case 1:
        func_800CE870(D_800F32D0->pool, 0, (s16 *)&joint);
        radius = params->radius;
        period = params->period;
        if (D_800E27EC <= params->lastFrame) {
            if (D_800E27EC % period == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->position.x;
                    child->y = target->position.y;
                    child->z = target->position.z;
                    phase = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(phase) * reach / 4096;
                    child->y -= func_80071A54() & 0x1F;
                    child->z += rsin(phase) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 32 - 16;
                    child->vy = -(func_80071A54() % 32);
                    child->vz = func_80071A54() % 32 - 16;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (D_800E27EC % period == period / 2) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->position.x;
                    child->y = target->position.y;
                    child->z = target->position.z;
                    phase = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(phase) * reach / 4096;
                    child->y -= func_80071A54() & 0x1F;
                    child->z += rsin(phase) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 24 - 12;
                    child->vy = -(func_80071A54() % 24);
                    child->vz = func_80071A54() % 24 - 12;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (!(D_800E27EC & 1) && !(func_80071A54() & 1)) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = joint.x;
                child->y = joint.y;
                child->z = joint.z;
                child->x += (func_80071A54() & 0x1FF) - 0x100;
                child->y += func_80071A54() % 800 - 400;
                child->z += (func_80071A54() & 0x1FF) - 0x100;
                child->size = (func_80071A54() & 0x1FF) + 0x2AA;
                child->angle = func_80071A54();
                child->vx = func_80071A54() % 24 - 12;
                child->vy = -(func_80071A54() % 24);
                child->vz = func_80071A54() % 24 - 12;
                child->state = 2;
                child->timer = 0;
            }
        }
        if (D_800E27EC == params->lastFrame + 16 && target->sound != -1) {
            func_800866A4(target->sound, 0);
        }
        if (D_800E27EC < 16) break;
        return 2;
    case 2:
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        {
            int tpage = D_800E2850[D_800E11E8];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 4;
            D_800F3368.tpage = tpage;
        }
        size = 0x2AA;
        phase = (func_80071A54() & 0x1F) + 0x20;
        func_800CE8F0(D_800F32D0->pool, 13, &rotation, &anchor);
        {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&anchor, 0, size, size,
                          (s16)D_800F3368.parameter02 + 0xD8, clut, 1, phase,
                          &color);
        }
        func_800CE8F0(D_800F32D0->pool, 13, &rotation2, &anchor);
        {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&anchor, 0, size, size,
                          (s16)D_800F3368.parameter02 + 0xD8, clut, 1, phase,
                          &color);
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
