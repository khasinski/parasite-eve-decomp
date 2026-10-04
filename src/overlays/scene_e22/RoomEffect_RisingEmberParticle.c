#include "pe1/scene_e22_ember_burst.h"

/* Rising ember particle: an ember drifts on a slowly turning circle while
 * it bounces off the floor (state 0), sparks coast to rest (states 1 and
 * 2); each draws as a turning glow. */
int func_8019485C(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin;
    RenderColor color = D_8018F204;
    int fall;
    int bounce;
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
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y;
            p->heading.y = fall;
            if (p->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            p->x += func_80077CF4((s16)p->timer << 8) * 40 / 4096;
            p->y += func_80077DC4((s16)p->timer << 8) * 40 / 4096;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = func_80077DC4((s16)p->timer << 5) / 2 + 0x800;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                spin.x = 0;
                spin.y = 0;
                D_800F3368.tpage = tpage;
            }
            spin.z = (s16)p->timer * 32;
            spin.flags = 0;
            func_800CF3AC(D_80199388, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size, size,
                          (s16)D_800F3368.parameter02 + 0x64,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                         : palette + 4),
                          1, 0x80, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            size = func_80077CF4((s16)p->timer << 6) + 0x800;
            glow = func_80077DC4((s16)p->timer * 1204 / 16) / 128;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                spin.x = 0;
                spin.y = 0;
                D_800F3368.tpage = tpage;
            }
            spin.z = (s16)p->timer * 60;
            spin.flags = 0;
            func_800CF3AC(D_80199388, &color, ((s16)p->timer << 5) / 12);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size * 2, size * 2, 0x40,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                         : palette + 2),
                          1, glow, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11EA.index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                spin.x = 0;
                spin.y = 0;
                D_800F3368.tpage = tpage;
            }
            spin.z = (s16)p->timer * 60;
            spin.flags = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, 0x1800, 0x1800,
                          (s16)D_800F3368.parameter02 * (s16)p->timer + 0x80,
                          func_80077AA4(0, palette), 1, 0x40, 0);
            break;
        }
        }
        break;
    }
    return 0;
}
