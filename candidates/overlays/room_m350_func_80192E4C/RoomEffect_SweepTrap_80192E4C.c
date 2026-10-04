#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_m350_sweep_trap.h"

typedef union RoomM350Angles {
    GteShortVector angles;
    GteRotation rotation;
} RoomM350Angles;

typedef union RoomM350Step {
    GteShortVector vector;
    s32 words[2];
} RoomM350Step;

/* Sweeping beam trap: sprites swept down a column from the actor's head,
 * a flare and two fan blades towards the anchor, then a projected quad test
 * that tags the player once. */
int func_80192E4C(int mode, RoomM350SweepTrap *trap) {
    GteShortVector position;
    RoomM350Step step;
    RoomM350Angles rotation;
    RoomM350Angles spin;
    GteMatrix matrix;
    GteShortVector quad[4];
    GteMatrix frame;
    int outside;
    int count;
    int phase;
    RoomM350Trig *trig;
    int intensity;
    int dist;
    int i;
    int k;
    int c;
    int s;
    int dx;
    int dz;

    if (mode == 1) {
        if (D_800E27EC >= 0x10) return 1;
    } else if (mode == 2) {
        count = D_800E27EC;
        phase = count - 1;
        intensity = phase < 9;
        i = 8;
        if (intensity) i = phase;
        trig = D_800966EC;
        rotation.angles.x = -D_800966EC[(i * 128) & 0xFFF].sin * 0x260 / 4096 - 0x500;
        rotation.angles.y = D_800F32D0->actor->heading;
        rotation.angles.z = 0x400;
        RotMatrixYXZ(&rotation.angles, &matrix);
        matrix.t[0] = 0;
        matrix.t[1] = 0;
        matrix.t[2] = 0;
        step.words[0] = 0;
        step.vector.z = 0x70;
        gte_ldrotmatrix(&matrix);
        gte_ldtransmatrix(&matrix);
        gte_ldv0(&step.vector);
        gte_rt();
        gte_stsv(&step.vector);
        rotation.angles.pad = 1;
        rotation.angles.x += 0x400;
        position = *trap->anchor;
        if (intensity) intensity = 0x80;
        else intensity = trig[((count - 9) * 128) & 0xFFF].cos >> 5;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        for (i = phase; position.y < D_800942EC.count; i++) {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800CEE20(&position, &rotation.rotation, 0x800, 0x1800,
                          (s16)D_800F3368.parameter02 * (i & 7) + 0x40,
                          GetClut(0, palette), 1, intensity, &D_8019A434[D_800E27EC & 1]);
            position.x += step.vector.x;
            position.y += step.vector.y;
            position.z += step.vector.z;
        }
        spin.angles.x = 0x400;
        spin.angles.y = 0;
        spin.angles.pad = 1;
        spin.angles.z = D_800E27EC << 7;
        D_800F3368.tpage = D_800E2850[D_800E11FA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&position, &spin.rotation, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * 17 + 0x84,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, intensity, &D_8019A434[D_800E27EC & 1]);
        }
        dx = position.x - trap->anchor->x;
        dz = position.z - trap->anchor->z;
        dist = Math_IntSqrt(dx * dx + dz * dz);
        rotation.angles.z = 0;
        rotation.angles.x = 0;
        rotation.angles.y -= 0x14;
        for (i = 0; i < 2; i++) {
            func_800D0E88(&position, &rotation.rotation, dist, 0x200, &D_8019A43C, &D_8019A3C8,
                          &D_8019A3C8, intensity, 1);
            rotation.angles.y += 0x28;
        }
        spin.angles.x = 0;
        spin.angles.y = 0;
        spin.angles.pad = 0;
        spin.angles.z = D_800E27EC << 7;
        {
            int scale = D_800966EC[(phase * 64) & 0xFFF].sin + 0x1000;
            func_800D0728(trap->anchor, 0x100, 0x1C0, 0x10, &spin.rotation, scale, scale,
                          &D_8019A3C8, &D_8019A440,
                      D_800966EC[((D_800E27EC - 1) * 64) & 0xFFF].cos >> 5, 1);
        }
        if (trap->done) return 0;
        if (g_PlayerEntity->stance < 4) return 0;
        D_8019A444[3].z = -dist;
        D_8019A444[2].z = -dist;
        c = rcos(D_800F32D0->actor->heading);
        s = rsin(D_800F32D0->actor->heading);
        frame.m[0][2] = s;
        frame.m[2][0] = -s;
        frame.m[1][1] = 0x1000;
        frame.m[0][0] = c;
        frame.m[2][2] = c;
        frame.t[2] = 0;
        frame.t[1] = 0;
        frame.t[0] = 0;
        frame.m[2][1] = 0;
        frame.m[1][2] = 0;
        frame.m[1][0] = 0;
        frame.m[0][1] = 0;
        frame.t[0] = position.x;
        frame.t[1] = position.y;
        frame.t[2] = position.z;
        gte_ldrotmatrix(&frame);
        gte_ldtransmatrix(&frame);
        for (k = 0; k < 4; k++) {
            gte_ldv0(&D_8019A444[k]);
            gte_rt();
            gte_stsv(&quad[k]);
        }
        {
            GteShortVector *vertex;
            int player = (g_PlayerEntity->z << 16) | (u16)g_PlayerEntity->x;
            int prev = (quad[3].z << 16) | (u16)quad[3].x;
            k = 0;
            vertex = quad;
            while (k < 4) {
                int cur = (vertex->z << 16) | (u16)vertex->x;
                gte_ldsxy0(player);
                gte_ldsxy2(prev);
                gte_ldsxy1(cur);
                gte_nclip();
                gte_stmac0(&outside);
                if (outside < 0) break;
                vertex++;
                k++;
                prev = cur;
            }
        }
        if (k < 4) return 0;
        g_PlayerEntity->actor->flags |= 0x4000;
        {
            RoomM350TrapNode *node = D_800F32D0->actor->node;
            if (node) node->flags |= 0x80000000;
        }
        trap->done = 1;
        D_8019A79C = 1;
    }
    return 0;
}
