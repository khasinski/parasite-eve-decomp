#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

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

/* Flash burst: while the scene event runs it plays the burst sounds, drops
 * falling sparks at frame 3 and two drifting rings up to frame 4, then
 * fades a glow sprite, halo and ring over 32 frames. */
int func_80195D20(int mode, RoomShakeBurstPoint *burst) {
    RenderColor glowColor;
    RenderColor color = D_8018F210;
    RoomOrbitTrailParticle *child;
    void **soundSlot;
    int volume;
    int time;
    int i;

    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 0, (s16 *)burst);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x10, func_80195904);
    case 1:
        if (D_800E2368->running == 0)
            return 1;
        if (D_800E27EC == 2) {
            soundSlot = &D_800B0E64;
            if (*soundSlot != 0) {
                volume = 0x7F;
                time = func_800D3FD8();
                func_8006DF50(*soundSlot, 0x5E7, time, 0x80, volume);
                if (*(void *volatile *)soundSlot != 0)
                    func_8006DF50(*soundSlot, 0x5E8, 0x80, 0x80, volume);
            }
        }
        if (D_800E27EC == 3) {
            for (i = 0; i < 3; i++) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54() % 50 - 0x19;
                    child->heading.y = func_80071A54() % 50 - 0x19;
                    child->heading.z = func_80071A54() % 50 - 0x19;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 5) {
            for (i = 0; i < 2; i++) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->radius = (func_80071A54() & 0x7FF) + 0x800;
                    child->x += (func_80071A54() & 0x7F) - 0x40;
                    child->y += (func_80071A54() & 0x7F) - 0x40;
                    child->z += (func_80071A54() & 0x7F) - 0x40;
                    child->heading.x = func_80071A54() % 190 - 0x5F;
                    child->heading.y = func_80071A54() % 190 - 0x5F;
                    child->heading.z = func_80071A54() % 190 - 0x5F;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 8) break;
        return 2;
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
            D_800F3368.depth = 0x20;
            D_800F3368.tpage = tpage;
        }
        if (D_800E27EC == 1)
            func_800D1AE0(&color, 0x80, 2, 8);
        else if (D_800E27EC == 2)
            func_800D1AE0(&color, 0x80, 3, 8);
        if (D_800E27EC < 0x21) {
            int clut;
            int kind;
            int palette;
            int angle = D_800E27EC << 5;
            int glow = func_80077DC4(angle) / 32;
            int scale;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x30, palette);
            func_800CEE20((GteShortVector *)burst, 0, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * (D_800E27EC / 4) + 0xA0,
                          clut, 1, glow, 0);
            func_800CF3AC(D_80199770, &glowColor, D_800E27EC);
            scale = func_80077CF4(angle) + 0x1000;
            if (D_800E27EC & 1)
                glow = glow * 2 / 3;
            func_800D004C((GteShortVector *)burst, 500, 500, 10, 0, scale, scale,
                          &glowColor, 0, glow, 1);
            glow = func_80077DC4(angle) / 32;
            scale = func_80077CF4(angle) + 0x800;
            func_800D0728((GteShortVector *)burst, 0x44C, 0x6A4, 0x10, 0, scale,
                          scale, 0, &glowColor, glow / 2, 1);
        }
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x10;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
