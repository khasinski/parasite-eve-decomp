#include "menu_memcard_glint.h"

/* Spark that drifts with damped velocity for 44 frames; state 0 draws a
 * flickering coloured glow, state 1 a larger glow that fades with a sine. */
int Memcard_DriftGlowParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = D_801ED7FC;
    RenderColor color;
    int angle;
    int size;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vy = spark->vy * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            if ((s16)spark->timer < 44) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 44;
            fade = 128 - ((D_800E27EC & 1) << 5);
            func_800CF3AC(D_801F1BB0, &color, (s16)spark->timer);
            rotation.z = -D_800E27EC * 16;
            size = rcos(angle) + 0x1000;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[D_800E11E6];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)spark, &rotation, size, size, 128,
                          func_80077AA4(32, palette), 1, fade, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 44;
            fade = rsin(angle * 2) / 32;
            rotation.z = -D_800E27EC * 16;
            size = rsin(angle) / 2 + 0x1000;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[D_800E11F6];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 5;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)spark, &rotation, size * 3 / 2, size * 3 / 2, 64,
                          func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3),
                          1, fade / 2, 0);
            break;
        }
        }
        break;
    }
    return 0;
}
