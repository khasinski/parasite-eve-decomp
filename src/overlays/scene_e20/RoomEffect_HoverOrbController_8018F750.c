/* MASPSX_FLAGS: --expand-div */
#include "pe1/scene_e20_hover_orb.h"

/* Hover orb controller: mode 0 loads the orb model at the actor; mode 1
 * glides it to random spots around the actor until the script hands it a
 * target, then flies at the target, bursts into a fan of sparks and hurts
 * the actor in reach; mode 2 draws the beam, the model and its glow. */
int func_8018F750(int mode, SceneE20HoverOrb *orb, SceneE20HoverTarget *target)
{
    GteShortVector point;
    GteShortVector rotation;
    SceneE20HoverColor beamColor = D_8018EFFC;
    SceneE20HoverColor edgeColor = D_8018F000;
    SceneE20HoverColor flashA = D_8018F004;
    SceneE20HoverColor flashB = D_8018F008;
    GteMatrix matrix;
    GteVector scale;
    SceneE20HoverMatrixSlot *slot;
    SceneE20HoverSpark *spark;
    SceneE20HoverObject *object;
    SceneE20HoverChannel *channel;
    SceneE20HoverColor *flash;
    u16 *index;
    int weight;
    int t;
    int i;
    int glow;
    int size;

    switch (mode) {
    case 0:
        orb->state = 0;
        orb->timer = 0;
        orb->intensity = 0x80;
        target->pending = 0;
        orb->destination.x = D_800F32D0->pool->position.x;
        orb->destination.y = D_800F32D0->pool->position.y;
        orb->destination.z = D_800F32D0->pool->position.z;
        orb->origin.x = orb->destination.x;
        orb->origin.y = orb->destination.y;
        orb->origin.z = orb->destination.z;
        orb->position.x = orb->destination.x;
        orb->position.y = orb->destination.y;
        orb->position.z = orb->destination.z;
        D_8019085C = func_8006E498(D_800B0E64.base, 0xC5543704);
        func_800C6D5C(D_8019085C, 0, 0);
        func_800D1384(&orb->position, &orb->position, 0x3EE, &beamColor, &edgeColor, 0x80,
                      orb->beam, 1);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20, func_8018F028);
    case 1:
        switch (orb->state) {
        case 0:
            target->pending = 0;
            func_800CE870(D_8009D254, 0, &point);
            point.x = (func_80071A54() & 0x7FF) - 0x400;
            point.y = D_800942EC.count - 600 - (func_80071A54() & 0x1FF);
            point.z = (func_80071A54() & 0x7FF) - 0x400;
            orb->destination.x = point.x;
            orb->destination.y = point.y;
            orb->destination.z = point.z;
            func_800CE870(D_8009D254, 1, &orb->target);
            orb->duration = (func_80071A54() & 0xF) + 6;
            orb->timer = 0;
            orb->state = 1;
            break;
        case 1:
            if (++orb->timer == 1 && (func_80071A54() & 1)) {
                func_8006DCE4(0x5FF, func_800D3FD8(), orb->position.x, orb->position.y,
                              orb->position.z);
            }
            t = func_80077CF4((orb->timer << 10) / orb->duration);
            LoadAverageShort12(&orb->origin, &orb->destination, 0x1000 - t, t, &orb->position);
            func_800CE870(D_8009D254, 1, &orb->target);
            if (orb->timer >= orb->duration) {
                orb->state = 2;
                orb->timer = 0;
            }
            break;
        case 2:
            orb->timer++;
            func_800CE870(D_8009D254, 1, &orb->target);
            if (orb->timer >= 4) {
                orb->state = 0;
                orb->timer = 0;
                orb->origin.x = orb->position.x;
                orb->origin.y = orb->position.y;
                orb->origin.z = orb->position.z;
                if (target->pending) {
                    target->pending = 0;
                    orb->state = 3;
orb->destination.x = target->rotation.x; orb->destination.y = target->rotation.y; orb->destination.z = target->rotation.z;
orb->targetDestination.x = target->destination.x; orb->targetDestination.y = target->destination.y; orb->targetDestination.z = target->destination.z;
orb->duration = target->duration;
orb->targetOrigin.x = orb->target.x; orb->targetOrigin.y = orb->target.y; orb->targetOrigin.z = orb->target.z;
                }
            }
            break;
        case 3:
            if (++orb->timer == 1) {
                func_8006DCE4(0x5FF, func_800D3FD8(), orb->position.x, orb->position.y,
                              orb->position.z);
            }
            t = func_80077CF4((orb->timer << 10) / orb->duration);
            weight = 0x1000 - t;
            LoadAverageShort12(&orb->origin, &orb->destination, weight, t, &orb->position);
            LoadAverageShort12(&orb->targetOrigin, &orb->targetDestination, weight, t,
                               &orb->target);
            if (orb->timer >= orb->duration) {
                orb->state = 4;
                orb->timer = 0;
                orb->origin.x = orb->position.x;
                orb->origin.y = orb->position.y;
                orb->origin.z = orb->position.z;
            }
            break;
        case 4:
            if (++orb->timer == 1) {
                spark = func_800CE610(D_800F33E0->pool);
                if (spark) {
                    spark->heading.x = orb->rotation.x;
                    spark->heading.y = orb->rotation.y;
                    spark->heading.z = orb->rotation.z;
                    func_800CFB7C(&orb->rotation, 0x8C, &spark->position);
                    spark->position.x += orb->position.x;
                    spark->position.y += orb->position.y;
                    spark->position.z += orb->position.z;
                    spark->state = 0;
                    spark->timer = 0;
                }
                func_8006DCE4(0x600, 0x80, orb->position.x, orb->position.y, orb->position.z);
            }
            if (orb->timer >= 4) {
                orb->state = 5;
                orb->timer = 0;
                spark = func_800CE610(D_800F33E0->pool);
                if (spark) {
                    spark->position.x = orb->target.x;
                    spark->position.y = orb->target.y;
                    spark->position.z = orb->target.z;
                    spark->heading.x = orb->rotation.x;
                    spark->heading.y = orb->rotation.y;
                    spark->heading.z = orb->rotation.z;
                    spark->state = 1;
                    spark->timer = 0;
                }
                for (i = 0; i < 12; i++) {
                    spark = func_800CE610(D_800F33E0->pool);
                    if (spark) {
                        spark->position.x = orb->target.x;
                        spark->position.y = orb->target.y;
                        spark->position.z = orb->target.z;
                        spark->heading.x = func_80071A54() % 70 - 35;
                        spark->heading.y = -(func_80071A54() % 70);
                        spark->heading.z = func_80071A54() % 70 - 35;
                        spark->state = 2;
                        spark->timer = 0;
                    }
                }
            }
            break;
        case 5:
            orb->timer++;
            if (func_800C6B90(&orb->target, 0x42) && D_80190800 == 0) {
                object = D_800F32D0->pool->object;
                object->actions[(object->flags >> 17) & 0x70] = 2;
                if (D_800E2368->active) {
                    channel = D_800F32D0;
                    if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        channel->pool->object->flags =
                            (channel->pool->object->flags & 0xC0FFFFFF) | 0x2F000000;
                        channel->pool->object->flags |= 0x80000000;
                    }
                }
                D_80190800 = 0x74;
            }
            if (orb->timer >= 8) {
                orb->state = 0;
                orb->timer = 0;
            }
            break;
        case 6:
            orb->timer++;
            orb->intensity = 0x60 - orb->timer * 3;
            if (orb->timer >= 0x20) return 1;
            break;
        }
        if (orb->state != 6) {
            orb->intensity = func_80077CF4(D_800E27EC << 5) / 128 + 0x80;
        }
        func_800CFAA8(&orb->position, &orb->target, &orb->rotation);
        func_800CFB7C(&orb->rotation, 0x8C, &orb->flareA);
        orb->flareA.x += orb->position.x;
        orb->flareA.y += orb->position.y;
        orb->flareA.z += orb->position.z;
        func_800CFB7C(&orb->rotation, -0x8C, &orb->flareB);
        orb->flareB.x += orb->position.x;
        orb->flareB.y += orb->position.y;
        orb->flareB.z += orb->position.z;
        if (D_800F32D0->pool->object->health < 0x7A120 && orb->state != 6) {
            orb->state = 6;
            orb->timer = 0;
        }
        if (D_80190800 > 0) {
            D_80190800--;
            return 0;
        }
        D_80190800 = 0;
        return 0;
    case 2:
        slot = &D_800BCFA4;
        gte_ldrotmatrix(slot->value);
        gte_ldtransmatrix(slot->value);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 4;
            D_800F3368.tpage = tpage;
        }
        gte_ldrotmatrix(slot->value);
        gte_ldtransmatrix(slot->value);
        flash = (D_800E27EC & 1) ? &flashA : &flashB;
        switch (orb->state) {
        case 4:
            t = orb->timer << 10;
            LoadAverageShort12(&orb->flareA, &orb->target, 0x1000 - t, t, &point);
            func_800D2B58(&orb->flareA, &point, flash, flash, 0x80, 0x80, 1);
            break;
        case 5:
            glow = func_80077DC4(orb->timer << 7) / 32;
            func_800D2B58(&orb->flareA, &orb->target, flash, flash, glow / 2, glow, 1);
            break;
        }
        if (orb->intensity != 0) {
            /* t doubles as the blend mode: opaque at high intensity. */
            t = 1;
            if (orb->intensity >= 0x60) t = 0xFF;
            index = &D_800E11FA;
            {
                int tpage = D_800E2850[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            glow = (u16)(D_800E2850[*index] | GetTPage(0, t, 0, 0));
            size = 0x400;
            {
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                GsSetOrign(glow, func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 6
                                                                          : palette + 2));
            }
            if (t != 0xFF) func_800C6ED8(1);
            else func_800C6ED8(0);
            rotation.x = orb->rotation.x;
            rotation.y = orb->rotation.y;
            rotation.z = orb->rotation.z;
            rotation.x += 0x800;
            rotation.z = func_80077CF4(D_800E27EC * 0x60) / 8 + 0x800;
            /* The spun rotation above is built but the matrix uses the
             * heading itself. */
            RotMatrixYXZ(&orb->rotation, &matrix);
            matrix.t[0] = orb->position.x;
            matrix.t[1] = orb->position.y;
            matrix.t[2] = orb->position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            Gte_ScaleMatrix(&matrix, &scale);
            func_800C6EF8(D_8019085C);
            func_800C6FA0(D_8019085C, orb->intensity);
            func_800C71E4(D_8019085C, &matrix);
            func_800C6F4C(D_8019085C);
            func_800D1384(&orb->flareA, &orb->flareB, 6, &beamColor, &edgeColor, orb->intensity,
                          orb->beam, 1);
            D_800F3368.depth = 0x33;
            glow = orb->intensity;
            if (D_800E27EC & 1) glow = glow * 3 / 4;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 9;
                else palette += 5;
                func_800CEE20(&orb->position, 0, 0x2000, 0x2000, 6, func_80077AA4(0, palette), 3,
                              glow, &beamColor);
            }
        }
        D_800F3368.depth = 0xB;
        break;
    }
    return 0;
}
