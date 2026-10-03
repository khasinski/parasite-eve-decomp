#include "pe1/scene_e22_ring_burst.h"

/* Twisting trail particle: for 32 frames it turns its twist angle and
 * draws a glow at the end of a bent trail that swings out and back. */
int func_801962FC(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1F4;
    GteShortVector offset;
    RenderColor color;
    int angle;
    int size;

    switch (mode) {
    case 1:
        if (p->state != 0) return 0;
        p->timer++;
        p->heading.pad += 0x20;
        if ((s16)p->timer < 0x20) break;
        return 1;
    case 2: {
        int kind;
        int palette;
        if (p->state != 0) return 0;
        angle = (s16)p->timer << 5;
        func_800CF844(p, &offset, 200, &p->heading,
                      func_80077CF4(angle) * p->radius / 4096, p->heading.pad);
        size = func_80077DC4(angle) * 3 / 2;
        spin.z = p->heading.pad - (s16)p->timer * 30;
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
        func_800CF3AC(D_8019944C, &color, (s16)p->timer);
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        func_800CEE20(&offset, &spin, size, size, (s16)D_800F3368.parameter02 + 100,
                      func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 10
                                                                      : palette + 6),
                      1, 0x80, &color);
        break;
    }
    }
    return 0;
}
