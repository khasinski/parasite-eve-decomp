#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

/* Attaches the burst to the first live actor with the given ids, or
 * reports the missing actor. */
static inline void RoomEffect_AttachEmberBurst(int subId, int typeId,
                                               RoomEmberBurst *burst) {
    FieldActor *actor;

    for (actor = D_8009D20C; actor != 0; actor = actor->next) {
        if (actor != D_8009D254 && actor->state != 0 &&
            actor->state->control10.command_value > 0 &&
            actor->sub_id == subId && actor->type_id == typeId) {
            func_800CE870((char *)actor, 0, (s16 *)burst);
            return;
        }
    }
    func_80071A74(D_8018F1CC, subId, typeId);
}

/* Ember burst: glows up for 16 frames, then for 24 frames scatters
 * orbiting sparks, embers on odd frames and smoke on even ones, then fades;
 * while it glows it draws a flickering halo and a ring. */
int func_80194AAC(int mode, RoomEmberBurst *burst, RoomEmberBurstParams *params) {
    RenderColor ringColor = D_8018F200;
    RenderColor haloColor = D_8018F204;
    RoomOrbitTrailParticle *child;
    int glow;

    switch (mode) {
    case 0:
        burst->state = 0;
        burst->timer = 0;
        burst->glow = 0;
        RoomEffect_AttachEmberBurst(params->subId, params->typeId, burst);
        func_800D3F64(0x5F5, func_800D3FD8());
        func_800D3F64(0x5F6, 0x80);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28, func_801944E8);
    case 1:
        switch (burst->state) {
        case 0:
            burst->timer++;
            burst->glow = func_80077CF4(burst->timer << 6) / 32;
            if (burst->timer < 0x10) break;
            burst->state = 1;
            burst->timer = 0;
            break;
        case 1:
            burst->timer++;
            if (burst->timer < 0x19) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->x += (func_80071A54() & 0x3F) - 0x20;
                    child->y += (func_80071A54() & 0x3F) - 0x20;
                    child->z += (func_80071A54() & 0x3F) - 0x20;
                    child->radius = (func_80071A54() & 0x1FF) + 0x200;
                    child->heading.x = func_80071A54();
                    child->heading.y = func_80071A54();
                    child->heading.z = func_80071A54();
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (burst->timer & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->y += 0x100 - (func_80071A54() & 0x1FF);
                    child->x += (func_80071A54() & 0xFF) - 0x80;
                    child->z += (func_80071A54() & 0xFF) - 0x80;
                    child->heading.x = func_80071A54() % 32 - 0x10;
                    child->heading.y = func_80071A54() % 8 - 4;
                    child->heading.z = func_80071A54() % 32 - 0x10;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (!(burst->timer & 1)) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->x += (func_80071A54() & 0x1FF) - 0x100;
                    child->y += (func_80071A54() & 0x1FF) - 0x100;
                    child->z += (func_80071A54() & 0x1FF) - 0x100;
                    child->state = 2;
                    child->timer = 0;
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
            int tpage = D_800E2850[D_800E11EA];
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
            func_800D0728((GteShortVector *)burst, 500, 700, 0x18, 0, scale, scale, 0,
                          &ringColor, glow / 2, 3);
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
