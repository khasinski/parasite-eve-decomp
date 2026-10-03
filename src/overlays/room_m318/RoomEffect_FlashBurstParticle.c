#include "pe1/room_shake_burst.h"

/* Flash burst particle: drifting rings slow down, falling sparks slow down
 * and bounce on the floor; both draw as spinning glow sprites that swell
 * and fade over 24 frames. */
int func_80195904(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin;
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
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = func_80077CF4(angle) + 0x800;
            size = size * p->radius / 4096;
            glow = func_80077DC4(angle) / 32 + 0x28;
            spin.x = 0;
            spin.y = 0;
            spin.z = (s16)p->timer * 32;
            spin.flags = 0;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size, size, 6,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                    : palette + 4),
                          1, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = func_80077CF4(angle) + 0x1000;
            glow = func_80077CF4(angle * 2) / 32;
            spin.x = 0;
            spin.y = 0;
            spin.z = (s16)p->timer * 14;
            spin.flags = 0;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            palette = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                : palette + 2);
            func_800CEE20((GteShortVector *)p, &spin, size * 3, size * 3, 0,
                          (u16)palette, 1, glow, 0);
            break;
        }
        }
        break;
    }
    return 0;
}
