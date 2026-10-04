#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

/* Particle-only declarations shared with the shake burst renderer. */
extern RenderColor D_8018F1FC;
extern u8 D_80199674[];

/* Ember burst particle: embers drift up and bounce off the floor, flashes
 * pulse in place, and orbiting sparks sweep around their anchor drawing
 * the glow track; smoke rises and fades. */
int func_801944E8(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1F4;
    GteShortVector offset;
    RenderColor color = D_8018F1FC;
    int fall;
    int bounce;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            p->y -= 8;
            if ((s16)p->timer < 0x10) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            size = func_80077CF4(angle) / 2 + 0x800;
            glow = func_80077DC4(angle) / 32;
            spin.z = -((s16)p->timer * 32);
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
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size, 0xDC,
                          GetClut(0x30, palette), 1, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            func_800CFB7C(&p->heading,
                          (s16)(func_80077DC4(angle) * p->radius / 4096), &offset);
            offset.x += p->x;
            offset.y += p->y;
            offset.z += p->z;
            size = func_80077CF4(angle);
            spin.z = (s16)p->timer * 32;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_80199674, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&offset, &spin, size / 2, size / 2, 0x24,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                    : palette + 4),
                          1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            size = func_80077CF4(angle) + 0x1000;
            glow = func_80077DC4(angle) / 32;
            spin.z = -((s16)p->timer * 32);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size, size, 0x42,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                    : palette + 4),
                          1, glow / 2, 0);
            break;
        }
        }
        break;
    }
    return 0;
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
        RoomEffect_AttachToActor(params->subId, params->typeId, burst);
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
