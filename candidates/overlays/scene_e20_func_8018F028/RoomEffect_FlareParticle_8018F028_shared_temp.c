#include "pe1/scene_e20_flare.h"

/* Mode 1 ages the flare and, while it falls, moves and damps it and
 * bounces it off the floor; mode 2 draws a spinning glow with a streak,
 * a pulsing glow pair, or a fading spark. */
int func_8018F028(int mode, SceneE20Flare *p)
{
    GteRotation rotation;
    RenderColor color;
    RenderColor streakColor;
    RenderColor glowColor;
    int fall;
    int bounce;
    int scale;
    int angle;
    int glow;
    int time;
    int state;
    int special;

    streakColor = D_8018EFF4;
    glowColor = D_8018EFF8;
    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if (p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            if (p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->position.x += p->heading.x;
            p->position.y += p->heading.y;
            p->position.z += p->heading.z;
            p->heading.x = p->heading.x * 59 / 60;
            p->heading.z = p->heading.z * 59 / 60;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->position.y >= D_800942EC.count) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if (p->timer < 0x14) break;
            return 1;
        }
        break;
    case 2:
        state = p->state;
        switch (state) {
        case 0: {
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                special = D_800E2850[D_800E11EA[8]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = special;
            }
            angle = p->timer << 6;
            scale = func_80077CF4(angle);
            special = (u16)p->heading.x;
            rotation.x = special;
            special = (u16)p->heading.y;
            rotation.y = special;
            special = (u16)p->heading.z;
            rotation.z = special;
            rotation.flags = 1;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_80190804, &color, p->timer);
            special = 4;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == special && D_800F3428 != 0) palette += 8;
            else palette += 4;
            func_800CEE20(&p->position, &rotation, scale * 3, scale * 3, 4,
                          func_80077AA4(0, palette), 1, 0x80, &color);
            if (p->timer < 8) {
                int width = (p->timer / 2) << 5;
                int top = 0x40;
                glow = func_80077DC4(angle) / 32;
                func_800CF3AC(D_80190804, &color, p->timer << 1);
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 7;
                else palette += 3;
                func_800D2370(&p->position, &rotation, 500, 0x168, width, top, 0x1F, 0x1F,
                              func_80077AA4(0, palette), &streakColor, &streakColor,
                              (s16)glow, 1);
            }
            break;
        }
        case 1: {
            int kind;
            int palette;
            time = p->timer;
            angle = time << 6;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = p->timer * 32;
            rotation.flags = 1;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11EA[0]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&p->position, 0, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * (p->timer / 2) + 0x60,
                          func_80077AA4(0, palette), state, 0x80, 0);
            scale = func_80077DC4(angle) / 2 + 0x800;
            glow = func_80077CF4(time << 7) / 32;
            {
                int tpage = D_800E2850[D_800E11EA[8]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 9;
            else palette += 5;
            func_800CEE20(&p->position, 0, scale * 4, scale * 4, 6,
                          func_80077AA4(0, palette), 3, glow, &glowColor);
            break;
        }
        case 2:
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_80190804, &color, (p->timer << 4) / 20);
            func_800D2104(&p->position, &color, 0x80, 1);
            break;
        }
        break;
    }
    return 0;
}
