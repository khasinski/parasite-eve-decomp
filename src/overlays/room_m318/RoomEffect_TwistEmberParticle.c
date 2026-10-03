#include "pe1/room_ember_burst.h"

/* Twisting ember particle: embers fly out and bounce on the floor, sparks
 * twist along a bent trail, and smoke rises; each draws with the glow
 * track. */
int func_80198268(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1F4;
    GteShortVector offset;
    RenderColor color;
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
            p->heading.x = p->heading.x * 50 / 51;
            p->heading.z = p->heading.z * 50 / 51;
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
            p->heading.pad += 0x32;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            p->y -= 1;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = func_80077CF4((s16)p->timer << 6) / 2 + 0x1000;
            spin.z = -((s16)p->timer * 32);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11E8];
                D_800F3368.palette = 2;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_80199890, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size,
                          (s16)D_800F3368.parameter02 * ((s16)p->timer / 2) + 0xA0,
                          GetClut(0x30, palette), 1, 0x80, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            func_800CF844(p, &offset, 200, &p->heading,
                          func_80077CF4(angle) * p->radius / 4096, p->heading.pad);
            size = func_80077DC4(angle) * 3 / 2;
            spin.z = p->heading.pad + (s16)p->timer * 32;
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
            func_800CF3AC(D_80199890, &color, (s16)p->timer * 3 / 2);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&offset, &spin, size, size, 4,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 7
                                                                    : palette + 3),
                          1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = func_80077DC4(angle) + 0x1000;
            glow = func_80077DC4(angle) / 32;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.depth = 0x20;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_80199890, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, 0, size, size, 0,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                    : palette + 2),
                          1, glow / 2, &color);
            D_800F3368.depth = 4;
            break;
        }
        }
        break;
    }
    return 0;
}
