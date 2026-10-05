#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_spin_glow.h"

/* Mode 1 moves the glow with damped horizontal speed and gravity for 32
 * ticks; mode 2 draws a pulsing spinning sprite with a fainter halo. */
int func_800DE7A8(int mode, FieldSpinGlow *state)
{
    GteShortVector position;
    GteRotation rotation;
    int scale;
    int intensity;
    int palette;

    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        state->vy = state->vy + 1;
        state->vx = state->vx * 31 / 32;
        state->vz = state->vz * 31 / 32;
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        intensity = state->intensity - state->intensity * D_800E27EC / 32;
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + D_800E27EC * 8;
        scale = rsin(D_800E27EC * 32) + 0x2800;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale,
                      D_800F336A * (D_800E27EC / 4) + 0xC0, GetClut(64, palette), 1,
                      intensity, 0);
        scale = 0x1800;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale,
                      D_800F336A * (D_800E27EC / 8) + 0x60, GetClut(0, palette), 3,
                      intensity, 0);
        break;
    }
    return 0;
}

#include "common.h"
#include "pe1/field_glow_fountain.h"

/* Mode 0 anchors at the owner actor and opens a pool of 32 spin glows;
 * mode 1 throws two glows per frame for 16 frames and ends at frame 48;
 * mode 2 draws the rising shape quad, the opening ring and the band. */
int func_800DEA30(int mode, FieldGlowFountain *state)
{
    GteRotation rotation;
    GteShortVector vector;
    RenderColor black = D_800C22BC;
    FieldSpinGlow *glow;
    int intensity;
    int scale;
    int kind;
    int palette;
    int i;

    switch (mode) {
    case 0:
        state->seed = rand();
        state->x = D_800F32D0->actor->render_object.target_x;
        state->y = D_800F32D0->actor->render_object.target_y;
        state->z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 16, 32,
                             (FieldEffectCallback)func_800DE7A8);
    case 1:
        if (D_800E27EC < 16) {
            glow = func_800CE610(D_800F33E0->end);
            if (glow) {
                for (i = 0; i < 2; i++) {
                    vector.x = rand() % 48 - 24;
                    vector.z = -(rand() % 80);
                    vector.y = rand() % 48 - 32;
                    FieldEng_RotateVector(
                        (GteMatrixWords *)D_8009D254->render_object.matrices,
                        &vector, &vector);
                    glow->vx = vector.x;
                    glow->vy = vector.y;
                    glow->vz = vector.z;
                    glow->x = state->x + (rand() & 0xFF) - 0x80;
                    glow->y = state->y + (rand() & 0xFF) - 0x80;
                    glow->z = state->z + (rand() & 0xFF) - 0x80;
                    glow->intensity = (rand() & 0x3F) + 0x50;
                    glow->angle = rand();
                    state->seed += 0x955;
                }
            }
        }
        if (D_800E27EC >= 48)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter0A = 5;
        D_800F3368.parameter06 = 1;
        D_800F3368.depth = 0x40;
        if (D_800E27EC < 25) {
            state->y -= 6;
            intensity = rcos((D_800E27EC << 10) / 24) * 240 / 4096;
            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            rotation.z = D_800E27EC << 4;
            scale = 0x3000 - rcos((D_800E27EC << 10) / 12);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)state, &rotation, scale, scale, 0x40,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, intensity, 0);
            if (D_800E27EC < 9) {
                intensity = rcos(D_800E27EC << 7) / 32;
                func_800D004C((GteShortVector *)state, 100, 700, 16, &rotation,
                              scale, scale, &black, 0, intensity, 1);
            }
        }
        D_800F3368.depth = 0;
        if (D_800E27EC < 17) {
            intensity = 0x80 - (D_800E27EC - 4) * 8;
            vector.x = state->x;
            vector.y = state->y;
            vector.z = state->z;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 7;
            scale = rsin((D_800E27EC - 4) << 6) * 2;
            func_800D0728(&vector, 600, 700, 0x18, &rotation, scale, scale, 0,
                          &black, intensity, 1);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x18;
        break;
    }
    return 0;
}
