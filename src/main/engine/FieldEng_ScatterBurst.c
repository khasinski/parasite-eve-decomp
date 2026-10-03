/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

/* Scatter burst at the actor target: opens a pool of bouncing sprites
 * (func_800D5010), throws two per frame for six frames on a turning angle,
 * and draws a growing flash, a short ring and a ground ring. */
int func_800D5290(int mode, RenderArcingEmitter *state)
{
    GteRotation rotation;
    GteShortVector position;
    RenderColor color = D_800C22BC;
    RenderBouncingSprite *sprite;
    int speed;
    int intensity;
    int scale;
    int kind;
    int palette;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 16, 12,
                             (FieldAnimTaskCallback)func_800D5010);
    case 1:
        if (D_800E27EC < 6) {
            for (i = 0; i < 2; i++) {
                sprite = func_800CE610(D_800F33E0->end);
                if (sprite) {
                    sprite->x = state->position.x;
                    sprite->y = state->position.y;
                    sprite->z = state->position.z;
                    speed = (rand() & 15) + 43;
                    sprite->velocity_x = rsin(state->phase) * speed / 4096;
                    sprite->velocity_z = rcos(state->phase) * speed / 4096;
                    sprite->velocity_y = -(rand() & 7) - 12;
                    sprite->duration = (rand() & 3) + 17;
                    sprite->angle = rand();
                    state->phase += 0x955;
                }
            }
        }
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[8]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 100;
        state->position.y -= 6;
        intensity = rcos((D_800E27EC << 10) / 24) / 32;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 4;
        rotation.flags = 0;
        scale = 0x3000 - rcos((D_800E27EC << 10) / 12);
        i = (D_800E27EC << 3) / 12;
        if (i > 7)
            i = 7;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        func_800CEE20(&state->position, &rotation, scale, scale,
                      (s16)D_800F3368.parameter02 * i,
                      GetClut(0, (kind == 4 && D_800F3428) ? palette + 5 : palette + 1),
                      1, intensity, 0);
        if (D_800E27EC < 9) {
            intensity = rcos(D_800E27EC << 7) / 32;
            func_800D004C(&state->position, 250, 250, 12, 0, scale, scale,
                          &color, 0, intensity, 1);
        }
        D_800F3368.depth = 4;
        if ((unsigned)(D_800E27EC - 4) < 13) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = D_800E27EC << 7;
            rotation.flags = 1;
            intensity = 128 - ((D_800E27EC - 4) << 7) / 12;
            scale = rsin(((D_800E27EC - 4) << 10) / 12) * 2;
            position.y = D_800942EC;
            func_800D0728(&position, 250, 400, 12, &rotation, scale, scale, 0,
                          &color, intensity, 1);
        }
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 64;
        break;
    }
    return 0;
}
