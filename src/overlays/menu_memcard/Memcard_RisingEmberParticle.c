#include "menu_memcard_glint.h"

/* Spark with three phases: state 0 falls and bounces with a spinning
 * coloured glint, state 1 rises with jitter as a pulsing flame frame,
 * state 2 lies on the floor as a flat glow that fades with a sine. */
int Memcard_RisingEmberParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = D_801ED7FC;
    RenderColor color;
    GteShortVector position;
    GteRotation tilt;
    int fall;
    int bounce;
    int angle;
    int size;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 1:
            spark->timer++;
            spark->y -= (func_80071A54() & 7) + 5;
            spark->x += (func_80071A54() & 7) - 3;
            spark->z += (func_80071A54() & 7) - 3;
            if ((s16)spark->timer < 16) break;
            return 1;
        case 2:
            spark->timer++;
            if ((s16)spark->timer < 18) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)spark->timer;
            func_800CF3AC(D_801F1C28, &color, angle * 2);
            rotation.z = spark->x + D_800E27EC * 32;
            size = rsin(angle << 7);
            D_800F3368.parameter00 = 16;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 16;
            D_800F3368.extent_y = 16;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[D_800E11E6];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)spark, &rotation, size, size * 3 / 2, 141,
                          func_80077AA4(48, palette), 1, 128, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = (s16)spark->timer << 6;
            fade = rcos(angle) / 32;
            rotation.z = D_800E27EC << 5;
            size = rcos(angle);
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[D_800E11E6];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)spark, 0, size / 2, size / 2,
                          (s16)D_800F3368.parameter02 * (spark->timer & 3) + 104,
                          func_80077AA4(16, palette), 3, fade, 0);
            break;
        }
        case 2: {
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 18;
            fade = rsin(angle * 2) / 32;
            size = rsin(angle);
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[D_800E11F6];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 1;
            tilt.x = 0x400;
            tilt.y = 0;
            tilt.z = (s16)spark->timer * 48;
            tilt.flags = 1;
            position.x = spark->x;
            position.z = spark->z;
            position.y = D_800942EC.count;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            func_800CEE20(&position, &tilt, size * 3, size * 3, 68,
                          func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3),
                          1, fade / 2, 0);
            break;
        }
        }
        break;
    }
    return 0;
}
