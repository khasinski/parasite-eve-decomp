#include "common.h"
#include "pe1/room_bounce_glint.h"

/* Bouncing glint particle callback for the room_m086 controller: mode 1
 * moves it (drift, then fall and bounce), mode 2 draws it. Both draws read
 * the palette kind back from the parameter block, which keeps the block
 * address in a saved register for the parameter02 read after the clut call.
 * The switches have no default arms so the last draw falls straight into
 * the shared return label, which lets each phase 0 draw cross-jump only the
 * call. */

int func_801900CC(int mode, RoomBounceGlint *glint, int *size) {
    GteRotation rotation;
    int extent = *size;
    int intensity;

    switch (mode) {
    case 1:
        switch (glint->phase) {
        case 0:
            glint->timer++;
            glint->x += glint->vx;
            glint->y += glint->vy;
            glint->z += glint->vz;
            glint->vy++;
            if (glint->timer < 24) {
                break;
            }
            return 1;
        case 1: {
            int fall;

            glint->timer++;
            glint->x += glint->vx;
            glint->y += glint->vy;
            glint->z += glint->vz;
            glint->vx = glint->vx * 31 / 32;
            glint->vz = glint->vz * 31 / 32;
            fall = (u16)glint->vy + 4;
            glint->vy = fall;
            if ((s16)glint->y >= D_800942EC.height) {
                int bounce = -(s16)fall;

                glint->vy = bounce;
            }
            if (glint->timer >= 32) {
                return 1;
            }
            break;
        }
        default:
            return 0;
        }
        break;
    case 2:
        switch (glint->phase) {
        case 0:
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            intensity = func_80077DC4((glint->timer << 10) / 24) / 32;
            if (glint->flag != 0) {
                int kind;
                int palette;

                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20((GteShortVector *)glint, 0, extent, extent,
                              D_800F336A * (s16)(glint->timer / 6),
                              (u16)func_80077AA4(0, palette), 2, intensity, 0);
            } else {
                int kind;
                int palette;

                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20((GteShortVector *)glint, 0, extent * 3 / 2,
                              extent * 3 / 2,
                              D_800F336A * ((glint->timer / 2) & 3),
                              (u16)func_80077AA4(0, palette), 1, intensity, 0);
            }
            break;
        case 1: {
            int kind;
            int palette;

            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            rotation.z = D_800E27EC << 7;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)glint, &rotation, extent * 3 / 2,
                          extent * 3 / 2,
                          (s16)D_800F3368.parameter02 * glint->flag + 0x1A,
                          (u16)func_80077AA4(0x70, palette), 0xFF, 0x80, 0);
            break;
        }
        }
        break;
    }
    return 0;
}
