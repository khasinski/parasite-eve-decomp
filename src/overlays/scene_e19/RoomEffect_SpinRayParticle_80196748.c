#include "pe1/scene_e19_2_spin_ray.h"

/* Mode 1 spins and ages the ray (states return 1 when they finish);
 * mode 2 draws it as a growing ray, a sweeping blade or a glow sprite. */
int func_80196748(int mode, SceneSpinRay *ray)
{
    RenderColor color;
    RenderColor rayColor;
    RenderColor bladeHead;
    RenderColor bladeTail;
    GteShortVector offset;
    int time;
    int width;
    int glow;
    int angle;

    rayColor = D_8018F224;
    bladeHead = D_8018F228;
    bladeTail = D_8018F22C;
    switch (mode) {
    case 1:
        switch (ray->state) {
        case 0:
            ray->timer++;
            ray->spin.angles.x += ray->reach;
            ray->spin.angles.y += ray->spinY;
            ray->spin.angles.z += 0x18;
            break;
        case 1:
            ray->timer++;
            ray->spin.angles.y += ray->spinY;
            ray->spin.angles.x += 8;
            ray->spin.angles.z += 0x18;
            if ((s16)ray->timer < 0x30) break;
            return 1;
        case 2:
            ray->timer++;
            ray->spin.angles.x += 4;
            ray->spin.angles.y += 4;
            ray->spin.angles.z += 0x30;
            if ((s16)ray->timer < 0x10) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        switch (ray->state) {
        case 0:
            time = (s16)ray->timer;
            if (time >= 0x10) time = 0x10;
            width = func_80077CF4(time << 6) / 32;
            if (ray->timer & 1) glow = 0x5C;
            else glow = 0x80;
            func_800CF3AC(D_8019B3E4, &color, time);
            func_800D0E88(&D_8019B670, &ray->spin.rotation, 0x9C4, width, &rayColor, 0, 0,
                          (s16)glow, 1);
            break;
        case 1: {
            int kind;
            int palette;
            angle = func_80077DC4(((s16)ray->timer << 10) / 48);
            glow = ((s16)ray->timer << 7) / 48;
            width = func_80077CF4(((s16)ray->timer << 10) / 48) / 4 + 0x1F4;
            func_800CFB7C(&ray->spin.angles, (s16)(ray->reach * angle / 4096), &offset);
            offset.x += D_8019B670.x;
            offset.y += D_8019B670.y;
            offset.z += D_8019B670.z;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800D2370(&offset, &ray->spin.rotation, width, 0x8C, 0x80, 0x80, 0x80, 0x10,
                          (u16)func_80077AA4(0x70, palette), &bladeHead, &bladeTail,
                          (s16)glow, 1);
            break;
        }
        case 2: {
            int kind;
            int palette;
            width = func_80077DC4((s16)ray->timer << 6) * 2;
            func_800CF3AC(D_8019B3E4, &color, (s16)ray->timer);
            {
                int tpage;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                tpage = D_800E2850[D_800E11EA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            ray->spin.rotation.flags = 1;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&D_8019B670, &ray->spin.rotation, width, width, 0x40,
                          (u16)func_80077AA4(0, palette), 1, 0x80, &color);
            break;
        }
        }
        break;
    }
    return 0;
}
