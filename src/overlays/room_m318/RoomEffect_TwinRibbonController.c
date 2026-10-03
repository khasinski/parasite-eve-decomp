#include "pe1/room_twin_ribbon.h"
#include "pe1/gte.h"

/* Twin ribbon controller: two mirrored pairs of ribbons between bone
 * offsets on bones 8 and 21; every frame a pulse is shed from one of the
 * two parameter bones. Draws a glow at the first offset, the ribbons and
 * restores the parameter block. */
int func_80196E4C(int mode, RoomTwinRibbon *ribbon, RoomTwinRibbonParams *params) {
    GteShortVector from;
    GteShortVector to;
    GteShortVector root = D_8018F214;
    GteShortVector tip = D_8018F21C;
    GteShortVector fork = D_8018F224;
    GteShortVector shed = D_8018F22C;
    RenderColor colorA = D_8018F234;
    RenderColor colorB = D_8018F238;
    RenderColor colorC = D_8018F23C;
    RoomOrbitTrailParticle *pulse;
    int scale;
    int alpha;

    switch (mode) {
    case 0:
        ribbon->boneA = 6;
        ribbon->boneB = 0x13;
        ribbon->reserved04 = 0;
        ribbon->timer = 0;
        tip.x *= -1;
        fork.x *= -1;
        func_800CE8F0(D_800F32D0->pool, 8, &root, &from);
        func_800CE8F0(D_800F32D0->pool, 8, &tip, &to);
        func_800D1384(&from, &to, 0x3EC, &colorA, &colorB, 0x80, ribbon->trailA, 1);
        func_800CE8F0(D_800F32D0->pool, 8, &fork, &from);
        func_800D1384(&from, &to, 0x3EE, &colorC, &colorB, 0x80, ribbon->trailC, 1);
        tip.x *= -1;
        fork.x *= -1;
        func_800CE8F0(D_800F32D0->pool, 0x15, &root, &from);
        func_800CE8F0(D_800F32D0->pool, 0x15, &tip, &to);
        func_800D1384(&from, &to, 0x3EC, &colorA, &colorB, 0x80, ribbon->trailB, 1);
        func_800CE8F0(D_800F32D0->pool, 0x15, &fork, &from);
        func_800D1384(&from, &to, 0x3EE, &colorC, &colorB, 0x80, ribbon->trailD, 1);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x10, func_80196C48);
    case 1:
        ribbon->timer++;
        if (params->stop) return 2;
        if (!(ribbon->timer & 1))
            func_800CE8F0(D_800F32D0->pool, ribbon->boneA, &shed, &from);
        else
            func_800CE8F0(D_800F32D0->pool, ribbon->boneB, &shed, &from);
        pulse = func_800CE610(D_800F33E0->pool);
        if (!pulse) return 0;
        pulse->x = from.x;
        pulse->y = from.y;
        pulse->z = from.z;
        pulse->state = 0;
        pulse->timer = 0;
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        alpha = 0x8C;
        scale = 0x1000;
        if (D_800E27EC & 1) {
            scale = 0x2000;
            alpha = 0x64;
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x30;
            D_800F3368.tpage = tpage;
        }
        root.z = 0;
        func_800CE8F0(D_800F32D0->pool, 0, &root, &from);
        {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            clut = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2);
            func_800CEE20(&from, 0, scale * 3 >> 1, scale * 3 >> 1, 0, clut, 3, alpha, 0);
        }
        tip.x *= -1;
        fork.x *= -1;
        func_800CE8F0(D_800F32D0->pool, 8, &root, &from);
        func_800CE8F0(D_800F32D0->pool, 8, &tip, &to);
        func_800D1384(&from, &to, 4, &colorA, &colorB, 0x80, ribbon->trailA, 1);
        func_800CE8F0(D_800F32D0->pool, 8, &fork, &from);
        func_800D1384(&from, &to, 6, &colorC, &colorB, 0x80, ribbon->trailC, 1);
        tip.x *= -1;
        fork.x *= -1;
        func_800CE8F0(D_800F32D0->pool, 0x15, &root, &from);
        func_800CE8F0(D_800F32D0->pool, 0x15, &tip, &to);
        func_800D1384(&from, &to, 4, &colorA, &colorB, 0x80, ribbon->trailB, 1);
        func_800CE8F0(D_800F32D0->pool, 0x15, &fork, &from);
        func_800D1384(&from, &to, 6, &colorC, &colorB, 0x80, ribbon->trailD, 1);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.depth = 0x10;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
