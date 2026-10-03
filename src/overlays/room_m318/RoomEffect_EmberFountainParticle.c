#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

/* Ember fountain particle: embers fly out and bounce on the floor, orbiting
 * sparks ride the fountain centre while their heading turns, and smoke
 * rises; each draws with the fountain's glow track. */
int func_80197618(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1F4;
    GteShortVector offset;
    RenderColor color;
    RenderColor seed = D_8018F240;
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
            p->x = D_8019993C.x;
            p->y = D_8019993C.y;
            p->z = D_8019993C.z;
            p->heading.x += 0x1D;
            p->heading.y += 0xB;
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
            angle = (s16)p->timer << 6;
            size = func_80077DC4(angle);
            glow = func_80077CF4(angle) / 32;
            spin.z = -((s16)p->timer * 24);
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
            func_800CF3AC(D_80199868, &color, (s16)p->timer * 24 / 16);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size, 0xD8,
                          GetClut(0x20, palette), 1, glow, &color);
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
            size = func_80077CF4(angle) * 3 / 2;
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
            func_800CF3AC(D_80199868, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&offset, &spin, size, size, 6,
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
            angle = func_80077CF4(angle * 2);
            glow = angle / 32;
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
            func_800CEE20((GteShortVector *)p, 0, size, size, 0x42,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                    : palette + 4),
                          1, glow / 2, &seed);
            break;
        }
        }
        break;
    }
    return 0;
}
