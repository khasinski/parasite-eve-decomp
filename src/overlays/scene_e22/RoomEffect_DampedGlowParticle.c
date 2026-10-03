#include "pe1/scene_e22_ring_burst.h"

/* Damped glow particle: drifts with a slowing velocity and bounces on the
 * floor for 8 frames, drawing a pulsing glow sprite tinted by its colour
 * track. */
int func_801931B8(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin;
    RenderColor color;
    int fall;
    int bounce;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        if (p->state != 0) return 0;
        p->timer++;
        p->x += p->heading.x;
        p->y += p->heading.y;
        p->z += p->heading.z;
        p->heading.x = p->heading.x * 63 / 64;
        p->heading.z = p->heading.z * 63 / 64;
        fall = (u16)p->heading.y;
        p->heading.y = fall;
        if (p->y >= D_800942EC.count) {
            bounce = -(s16)fall;
            p->heading.y = bounce;
        }
        if ((s16)p->timer < 8) break;
        return 1;
    case 2:
        if (p->state != 0) return 0;
        angle = (s16)p->timer << 7;
        size = func_80077DC4(angle) / 2 + 0x800;
        glow = func_80077DC4(angle) / 32;
        spin.x = 0;
        spin.y = 0;
        spin.z = (s16)p->timer * 32;
        spin.flags = 0;
        func_800CF3AC(D_80199190, &color, (s16)p->timer * 2);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int kind = D_800F336C;
            int palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)p, &spin, size, size,
                          (s16)D_800F3368.parameter02 + 100,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 8 : palette + 4),
                          1, glow, &color);
        }
        break;
    }
    return 0;
}
