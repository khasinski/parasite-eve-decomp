#include "pe1/scene_e19_2_spin_ray.h"

/* Spin ray controller: for 48 frames it seeds spinning rays on odd frames,
 * casts one of its sixteen blades every fourth frame and fans out 26
 * blades on the first frame. While it runs it follows the room actor,
 * publishes that spot as the ray origin and draws a halo, two glows and
 * two rings there. */
int func_80196CCC(int mode, SceneSpinRayBurst *burst)
{
    GteRotation tilt = D_8018F210;
    GteShortVector position;
    RenderColor color;
    RenderColor ringColor = D_8018F230;
    GteShortVector floor;
    GteRotation flat;
    SceneSpinRay *child;
    int i;
    int scale;
    int glow;

    switch (mode) {
    case 0:
        burst->timer = 0;
        burst->blades = 0x10;
        func_800D3F64(0x5E1, func_800D3FD8());
        func_800D3F64(0x5E2, 0x80);
        return func_800CE560(D_800F33E0->pool, 0x10, 0x32, func_80196748);
    case 1:
        if (++burst->timer & 1) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->spin.angles.x = func_80071A54();
                child->spin.angles.y = func_80071A54();
                child->spin.angles.z = func_80071A54();
                child->timer = 0;
                child->state = 2;
            }
        }
        if (!(burst->timer & 3) && burst->blades > 0) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                func_800CFFAC(child);
                child->timer = 0;
                child->state = 0;
                child->reach = (func_80071A54() & 0x1F) - 0x10;
                child->spinY = (func_80071A54() & 0x1F) - 0x10;
            }
            burst->blades--;
        }
        if ((s16)burst->timer == 1) {
            for (i = 0; i < 26; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    func_800CFFAC(child);
                    child->timer = 0;
                    child->state = 1;
                    child->reach = (func_80071A54() & 0x1FF) + 600;
                    child->spinY = (func_80071A54() & 7) - 3;
                }
            }
        }
        if ((s16)burst->timer < 0x30) break;
        return 1;
    case 2:
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        func_800CE8F0(D_800F32D0->pool, 0, &tilt, &position);
        D_8019B670.x = position.x;
        D_8019B670.y = position.y;
        D_8019B670.z = position.z;
        if (burst->timer & 1) glow = 0x60;
        else glow = 0x80;
        scale = func_80077CF4(((s16)burst->timer << 10) / 48) / 2 + 0x800;
        func_800CF3AC(D_8019B404, &color, (s16)burst->timer);
        func_800D004C(&position, 800, 800, 12, 0, scale, scale, &color, 0, 100, 1);
        {
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&position, 0, scale, scale, 8, func_80077AA4(0x90, palette), 1,
                          glow, 0);
        }
        flat.x = 0x400;
        flat.y = 0;
        flat.z = 0;
        flat.flags = 1;
        floor.x = position.x;
        floor.z = position.z;
        floor.y = D_800942EC.count;
        {
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&floor, &flat, scale, scale, 8, func_80077AA4(0x10, palette), 3,
                          glow, 0);
        }
        glow = ((s16)burst->timer << 7) / 48;
        func_800D0728(&position, 0x44C, 0x1770, 0x10, 0, 0x1000, 0x1000, 0, &ringColor,
                      glow, 1);
        func_800D0728(&position, 1000, 0x4B0, 0x18, 0, 0x1000, 0x1000, &ringColor, 0,
                      glow, 1);
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
